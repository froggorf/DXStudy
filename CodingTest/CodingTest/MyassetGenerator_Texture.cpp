#include <algorithm>
#include <iostream>
#include <fstream>
#include <filesystem>
#include <string>
#include <vector>

namespace fs = std::filesystem;

class MyAssetGenerator
{
public:
    struct Config
    {
        std::string sourceFolder;      // PNG 파일들이 있는 폴더
        std::string outputFolder;      // MyAsset 파일을 생성할 폴더
        std::string assetClass;        // 예: "UTexture"
        std::string contentBasePath;   // 예: "C:\\Project\\DXStudy\\LeeEngine\\MyGame\\Content"
    };

    static void GenerateAssets(const Config& config)
    {
        // 출력 폴더 생성
        if (!fs::exists(config.outputFolder))
        {
            fs::create_directories(config.outputFolder);
        }

        int generatedCount = 0;

        // 소스 폴더의 모든 PNG 파일 찾기
        for (const auto& entry : fs::directory_iterator(config.sourceFolder))
        {
            if (entry.is_regular_file() && entry.path().extension() == ".PNG" || entry.path().extension() == ".png")
            {
                std::string fileName = entry.path().stem().string();
                std::string fullPath = entry.path().string();

                // MyAsset 파일 생성
                if (CreateMyAsset(config, fileName, fullPath))
                {
                    generatedCount++;
                    std::cout << "✅ Generated: " << fileName << ".myasset" << std::endl;
                }
                else
                {
                    std::cerr << "❌ Failed: " << fileName << std::endl;
                }
            }
        }

        std::cout << "\n🎉 Total " << generatedCount << " assets generated!" << std::endl;
    }

private:
    static bool CreateMyAsset(const Config& config, const std::string& assetName, const std::string& pngPath)
    {
        // 출력 파일 경로
        std::string outputPath = config.outputFolder + "\\" + assetName + ".myasset";

        // /Content 이후의 상대 경로 계산
        std::string relativePath = CalculateRelativePath(pngPath, config.contentBasePath);

        // JSON 형태의 MyAsset 파일 생성
        std::ofstream outFile(outputPath);
        if (!outFile.is_open())
        {
            return false;
        }

        outFile << "{\n";
        outFile << "    \"Class\": \"" << config.assetClass << "\",\n";
        outFile << "    \"Name\": \"" << assetName << "\",\n";
        outFile << "    \"FilePath\": \"" << relativePath << "\"\n";
        outFile << "}\n";

        outFile.close();
        return true;
    }

    static std::string CalculateRelativePath(const std::string& fullPath, const std::string& basePath)
    {
        // Windows 경로를 Unix 스타일로 변환
        std::string normalized = fullPath;
        std::replace(normalized.begin(), normalized.end(), '\\', '/');

        std::string normalizedBase = basePath;
        std::replace(normalizedBase.begin(), normalizedBase.end(), '\\', '/');

        // Content 이후의 경로 추출
        size_t contentPos = normalized.find("/Content/");
        if (contentPos != std::string::npos)
        {
            return normalized.substr(contentPos);
        }

        // Content를 찾지 못하면 전체 경로 반환
        return normalized;
    }
};

int main()
{
    // 설정
    MyAssetGenerator::Config config;

    config.sourceFolder = R"(C:\Project\DXStudy\LeeEngine\MyGame\Content\Resource\MyGame\Texture\Sanhwa)";
    config.outputFolder = R"(C:\Project\DXStudy\LeeEngine\MyGame\Content\MyGame\Texture\Sanhwa)";
    config.assetClass = "UTexture";
    config.contentBasePath = R"(C:\Project\DXStudy\LeeEngine\MyGame\Content)";

    std::cout << "🔧 MyAsset Generator Starting...\n" << std::endl;
    std::cout << "Source Folder: " << config.sourceFolder << std::endl;
    std::cout << "Output Folder: " << config.outputFolder << std::endl;
    std::cout << "Asset Class: " << config.assetClass << "\n" << std::endl;

    try
    {
        MyAssetGenerator::GenerateAssets(config);
    }
    catch (const std::exception& e)
    {
        std::cerr << "❌ Error: " << e.what() << std::endl;
        return 1;
    }

    std::cout << "\n✨ Done! Press any key to exit..." << std::endl;
    std::cin.get();

    return 0;
}
