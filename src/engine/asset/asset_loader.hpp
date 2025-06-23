#pragma once

#include <fstream>

#include "../render/shader.hpp"
#include "../render/sprite.hpp"
#include "../render/font.hpp"
#include "../system/project_asset.hpp"
#include <yaml-cpp/yaml.h>
#include "../system/project_asset.hpp"

#include "../system/gl.hpp"
#include <GL/glew.h>
#include <iostream>

#include <ft2build.h>
#include FT_FREETYPE_H



class AssetLoader {
public:

    template <typename T>
    static T LoadFromPath([[maybe_unused]] const std::string& path)
    {
        static_assert( "AssetLoader::LoadFromPaths<T> not implemented for this type");
        return nullptr;
    }


    template<>
    Sprite LoadFromPath<Sprite>(const std::string &path) {
        return Sprite(path);
    }

    template<>
    std::string LoadFromPath<std::string>(const std::string &path) {
        std::ifstream file(path.c_str()); // open the file

        if (!file.is_open()) {
            std::cerr << "Error: Could not open the file at: " << path << "\n";
            return {};
        }

        std::stringstream buffer;
        buffer << file.rdbuf(); // read the whole file into the stringstream

        std::string content = buffer.str(); // get string content
        file.close(); // close the file

        return content;
    }


    template<>
    Font LoadFromPath<Font>(const std::string &path) {
        // FreeType
        // --------
        FT_Library ft;
        // All functions return a value different than 0 whenever an error occurred
        if (FT_Init_FreeType(&ft)) {
            std::cout << "ERROR::FREETYPE: Could not init FreeType Library" << std::endl;
        }

        // find path to font
        const std::string &fontName = path;

        Font font;

        if (fontName.empty()) {
            std::cout << "ERROR::FREETYPE: Failed to load font_name" << std::endl;
            return font;
        }

        // load font as face
        FT_Face face;
        if (FT_New_Face(ft, fontName.c_str(), 0, &face)) {
            std::cout << "ERROR::FREETYPE: Failed to load font" << std::endl;
            return font;
        }
        // set size to load glyphs as
        FT_Set_Pixel_Sizes(face, 0, 48);

        // disable byte-alignment restriction
        glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

        // load first 128 characters of ASCII set
        for (unsigned char c = 0; c < 128; c++) {
            // Load character glyph
            if (FT_Load_Char(face, c, FT_LOAD_RENDER)) {
                std::cout << "ERROR::FREETYTPE: Failed to load Glyph" << std::endl;
                continue;
            }
            // generate texture
            unsigned int texture;
            glGenTextures(1, &texture);
            glBindTexture(GL_TEXTURE_2D, texture);
            glTexImage2D(
                GL_TEXTURE_2D,
                0,
                GL_RED,
                (int32_t) face->glyph->bitmap.width,
                (int32_t) face->glyph->bitmap.rows,
                0,
                GL_RED,
                GL_UNSIGNED_BYTE,
                face->glyph->bitmap.buffer);
            // set texture options
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            // now store character for later use
            Character character = {
                texture,
                glm::ivec2(face->glyph->bitmap.width, face->glyph->bitmap.rows),
                glm::ivec2(face->glyph->bitmap_left, face->glyph->bitmap_top),
                static_cast<unsigned int>(face->glyph->advance.x)
            };
            font.Characters.insert(std::pair<char, Character>(c, character));
        }

        glBindTexture(GL_TEXTURE_2D, 0);
        // destroy FreeType once we're finished
        FT_Done_Face(face);
        FT_Done_FreeType(ft);

        return font;
    }

    template<>
    ProjectAsset LoadFromPath<ProjectAsset>(const std::string &path) {
        const std::string value = LoadFromPath<std::string>(path);
        YAML::Node root = YAML::Load(value);
        if (auto project = root["project"]) {
            std::string name = project["name"] ? project["name"].as<std::string>() : "";
            YAML::Node scenes = project["scenes"];
            std::vector<std::string> scenesGuids;
            if (scenes.IsSequence()) {
                for (std::size_t i = 0; i < scenes.size(); ++i) {
                    scenesGuids.push_back(scenes[i].as<std::string>());
                }
            }
            return {name, scenesGuids};
        }
        return {"ERROR", std::vector<std::string>()};
    }
};