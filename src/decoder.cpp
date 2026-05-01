#include "decoder.h"
#include "base64.h"
#include <iostream>
#include <fstream>
#include <vector>
#include <map>
#include <regex>
#include <opencv2/opencv.hpp>

void decodeQrToJpg(const std::string &inputDir, const std::string &outputFilePath)
{
    // 1. QRコード画像ファイルを探索
    std::vector<std::string> qr_files;
    cv::glob(inputDir + "/*.png", qr_files, false);

    if (qr_files.empty())
    {
        std::cerr << "Error: No QR code images found in " << inputDir << std::endl;
        return;
    }

    std::cout << "Found " << qr_files.size() << " QR code files." << std::endl;

    cv::QRCodeDetector qr_detector;
    std::map<int, std::string> data_map;
    int total_chunks = 0;

    // 2. 各QRコードを読み取り & デコード
    for (const auto &file_path : qr_files)
    {
        cv::Mat img = cv::imread(file_path, cv::IMREAD_GRAYSCALE);
        if (img.empty())
            continue;

        std::string decoded_data = qr_detector.detectAndDecode(img);
        if (decoded_data.empty())
            continue;

        // 3. ヘッダーを解析
        std::smatch match;
        std::regex header_regex(R"(\[(\d+)/(\d+)\])");
        if (std::regex_search(decoded_data, match, header_regex) && match.size() == 3)
        {
            int current_num = std::stoi(match[1].str());
            total_chunks = std::stoi(match[2].str());

            // ヘッダー部分を削除してデータを格納
            data_map[current_num] = decoded_data.substr(match[0].length());
            std::cout << "Decoded chunk " << current_num << "/" << total_chunks << std::endl;
        }
    }

    // 4. データがすべて揃っているか確認
    if (data_map.size() != total_chunks || total_chunks == 0)
    {
        std::cerr << "Error: Missing some QR codes. Expected " << total_chunks << ", but found " << data_map.size() << "." << std::endl;
        return;
    }

    // 5. Base64文字列を再構築
    std::string full_base64_str;
    for (int i = 1; i <= total_chunks; ++i)
    {
        full_base64_str += data_map[i];
    }

    // 6. Base64デコード
    std::string decoded_bytes_str = base64_decode(full_base64_str);
    std::vector<char> decoded_bytes(decoded_bytes_str.begin(), decoded_bytes_str.end());

    // 7. ファイルに書き出し
    std::ofstream output_file(outputFilePath, std::ios::binary);
    if (!output_file.is_open())
    {
        std::cerr << "Error: Cannot create output file " << outputFilePath << std::endl;
        return;
    }
    output_file.write(decoded_bytes.data(), decoded_bytes.size());

    std::cout << "Decoding complete. File saved to " << outputFilePath << std::endl;
}