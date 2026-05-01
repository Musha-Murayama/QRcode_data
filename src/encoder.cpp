#include "encoder.h"
#include "QrCode.hpp"
#include "base64.h"
#include <iostream>
#include <fstream>
#include <vector>
#include <cmath>
#include <opencv2/opencv.hpp> // 画像保存のため

// QRコードのデータをcv::Matに変換して保存するヘルパー関数
void saveQrCodeAsImage(const qrcodegen::QrCode &qr, const std::string &filename, int scale)
{
    int border = 4;
    cv::Mat img((qr.getSize() + border * 2) * scale, (qr.getSize() + border * 2) * scale, CV_8UC1, cv::Scalar(255));

    for (int y = 0; y < qr.getSize(); y++)
    {
        for (int x = 0; x < qr.getSize(); x++)
        {
            if (qr.getModule(x, y))
            {
                cv::Rect rect((x + border) * scale, (y + border) * scale, scale, scale);
                cv::rectangle(img, rect, cv::Scalar(0), -1);
            }
        }
    }
    cv::imwrite(filename, img);
}

void encodeJpgToQr(const std::string &inputFilePath, const std::string &outputDir)
{
    // 1. ファイルをバイナリで読み込み
    std::ifstream file(inputFilePath, std::ios::binary);
    if (!file.is_open())
    {
        std::cerr << "Error: Cannot open file " << inputFilePath << std::endl;
        return;
    }
    std::vector<char> buffer(std::istreambuf_iterator<char>(file), {});

    // 2. Base64エンコード
    std::string base64_str = base64_encode(reinterpret_cast<const unsigned char *>(buffer.data()), buffer.size());

    // 3. データ分割
    // QRコードのバージョンと誤り訂正レベルMで安全に格納できる文字数を定義
    // ここでは例として1000文字に固定
    const int CHUNK_SIZE = 1000;
    int total_chunks = static_cast<int>(ceil(static_cast<double>(base64_str.length()) / CHUNK_SIZE));

    std::cout << "Input size: " << buffer.size() << " bytes" << std::endl;
    std::cout << "Base64 size: " << base64_str.length() << " chars" << std::endl;
    std::cout << "Creating " << total_chunks << " QR codes..." << std::endl;

    for (int i = 0; i < total_chunks; ++i)
    {
        // ヘッダー作成 [現在の連番/総数]
        std::string header = "[" + std::to_string(i + 1) + "/" + std::to_string(total_chunks) + "]";

        int start = i * CHUNK_SIZE;
        int length = std::min(CHUNK_SIZE, static_cast<int>(base64_str.length() - start));
        std::string chunk = base64_str.substr(start, length);

        std::string data_to_encode = header + chunk;

        // 4. QRコード生成
        const qrcodegen::QrCode::Ecc errCorLvl = qrcodegen::QrCode::Ecc::MEDIUM;
        const qrcodegen::QrCode qr = qrcodegen::QrCode::encodeText(data_to_encode.c_str(), errCorLvl);

        // ファイル名を生成 (例: qrcode_01.png)
        char filename[256];
        snprintf(filename, sizeof(filename), "/qrcode_%02d.png", i + 1);
        std::string output_path = outputDir + filename;

        // 5. 画像として保存
        saveQrCodeAsImage(qr, output_path, 5); // 5倍のスケールで保存
        std::cout << "Generated " << output_path << std::endl;
    }

    std::cout << "Encoding complete." << std::endl;
}