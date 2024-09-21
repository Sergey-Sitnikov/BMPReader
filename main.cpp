#include <iostream>
#include <fstream>
#include <windows.h>

class BMPReader {
private:
    BITMAPFILEHEADER fileHeader;
    BITMAPINFOHEADER infoHeader;
    std::ifstream file;
    unsigned char* pixels;
    int width, height;

public:
    BMPReader() : pixels(nullptr) {}

    bool openBMP(const std::string& fileName) {
        file.open(fileName, std::ios::binary);
        if (!file.is_open()) {
            return false;
        }

        // Читаем заголовок файла
        file.read(reinterpret_cast<char*>(&fileHeader), sizeof(BITMAPFILEHEADER));

        // Проверяем, что это BMP-файл
        if (fileHeader.bfType != 0x4D42) {
            file.close();
            return false;
        }

        // Читаем заголовок изображения
        file.read(reinterpret_cast<char*>(&infoHeader), sizeof(BITMAPINFOHEADER));

        // Проверяем, что это 24-битный или 32-битный BMP
        if (infoHeader.biBitCount != 24 && infoHeader.biBitCount != 32) {
            file.close();
            return false;
        }

        width = infoHeader.biWidth;
        height = infoHeader.biHeight;

        // Выделяем память для пикселей
        pixels = new unsigned char[width * height * (infoHeader.biBitCount / 8)];

        // Читаем пиксели
        file.seekg(fileHeader.bfOffBits, std::ios::beg);
        file.read(reinterpret_cast<char*>(pixels), width * height * (infoHeader.biBitCount / 8));

        return true;
    }

    void displayBMP() {
        for (int y = height - 1; y >= 0; y--) {
            for (int x = 0; x < width; x++) {
                int index = (y * width + x) * (infoHeader.biBitCount / 8);
                if (pixels[index] == 0 && pixels[index + 1] == 0 && pixels[index + 2] == 0) {
                    std::cout << ".";
                } else {
                    std::cout << "#";
                }
            }
            std::cout << std::endl;
        }
    }

    void closeBMP() {
        file.close();
        delete[] pixels;
    }
};

int main(int argc, char* argv[]) {
    if (argc != 2) {
        std::cerr << "Usage: drawBmp.exe <file_path>" << std::endl;
        return 1;
    }

    BMPReader reader;
    if (!reader.openBMP(argv[1])) {
        std::cerr << "Failed to open BMP file: " << argv[1] << std::endl;
        return 1;
    }

    reader.displayBMP();
    reader.closeBMP();

    return 0;
}
