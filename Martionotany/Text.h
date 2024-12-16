#pragma once
#include "freetype/freetype.h"
#include "ECS.h"

struct Character {
    uint textureID = 0;            // ID handle of the glyph texture
    ivec2 size = ivec2(0);          // Size of glyph
    ivec2 bearing = ivec2(0);      // Offset from baseline to left/top of glyph
    uint advance = 0;           // Offset to advance to next glyph
};

class Font
{
public:
    static vector<Font*> fonts;
    static FT_Library ft;

    string fileLocation;
    std::map<char, Character> characters{};
    uint minimumSize = 0, vao = 0, vbo = 0;
    int mininumVertOffset = 0, maxVertOffset = 0, vertDisp = 0;

    Font(string fileLocation, uint minimumSize) :
        fileLocation(fileLocation), minimumSize(minimumSize)
    {
        fonts.push_back(this);
    }

    bool Read(FT_Byte* data, FT_Long size)
    {
        FT_Face face;
        if (FT_New_Memory_Face(ft, data, size, 0, &face))
        {
            cout << "ERROR::FREETYPE: Failed to load font\n";
            return false;
        }

        FT_Set_Pixel_Sizes(face, 0, minimumSize);

        if (FT_Load_Char(face, 'X', FT_LOAD_RENDER))
        {
            cout << "ERROR::FREETYTPE: Failed to load Glyph\n";
            return false;
        }

        glPixelStorei(GL_UNPACK_ALIGNMENT, 1); // disable byte-alignment restriction

        for (byte c = 0; c < 128; c++)
        {
            // load character glyph 
            if (FT_Load_Char(face, c, FT_LOAD_RENDER))
            {
                cout << "ERROR::FREETYTPE: Failed to load Glyph\n";
                continue;
            }
            // generate texture
            uint texture;
            glGenTextures(1, &texture);
            glBindTexture(GL_TEXTURE_2D, texture);
            glTexImage2D(
                GL_TEXTURE_2D,
                0,
                GL_RED,
                face->glyph->bitmap.width,
                face->glyph->bitmap.rows,
                0,
                GL_RED,
                GL_UNSIGNED_BYTE,
                face->glyph->bitmap.buffer
            );
            // set texture options
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
            // now store character for later use
            Character character = {
                texture,
                ivec2(face->glyph->bitmap.width, face->glyph->bitmap.rows),
                ivec2(face->glyph->bitmap_left, face->glyph->bitmap_top),
                static_cast<uint>(face->glyph->advance.x)
            };
            characters.insert(std::pair<char, Character>(c, character));

            mininumVertOffset = min(mininumVertOffset, character.bearing.y - character.size.y); // Always negative
            maxVertOffset = max(mininumVertOffset, character.bearing.y); // Always positive
        }
        vertDisp = maxVertOffset - mininumVertOffset; // Very useful.

        glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

        FT_Done_Face(face);
        FT_Done_FreeType(ft);

        glGenVertexArrays(1, &vao);
        glGenBuffers(1, &vbo);
        glBindVertexArray(vao);
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        float position[] = {
            0.0f, 0.0f,
            0.0f, 1.0f,
            1.0f, 1.0f,

            0.0f, 0.0f,
            1.0f, 1.0f,
            1.0f, 0.0f
        };
        glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6 * 2, position, GL_DYNAMIC_DRAW);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), 0);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);

        return true;
    }

#pragma region Text Width stuff
    int CharAdvance(char character)
    {
        return characters[character].advance >> 6; // bitshift by 6 to get value in pixels (2^6 = 64)
    }

    inline float CharAdvanceTrue(char character)
    {
        return CharAdvance(character) / (float)minimumSize;
    }

    int CharWidth(char character)
    {
        return characters[character].size.x; // Doesn't need to be bitshifted.
    }

    float CharWidthTrue(char character)
    {
        return CharWidth(character) / (float)minimumSize;
    }

    int TextWidth(string text)
    {
        uint result = 0;

        for (int i = 0; i < text.size() - 1; i++)
            result += CharAdvance(text[i]);
        result += CharWidth(text[text.size() - 1]);

        return result;
    }

    float TextWidthTrue(string text)
    {
        return TextWidth(text) / (float)minimumSize;
    }
#pragma endregion

    void Render(string text, vec2 pos, float scale, vec4 color)
    {
        scale /= minimumSize;
        float xOffset = 0;
        // activate corresponding render state
        glUseProgram(textShader.program);
        glUniform4f(glGetUniformLocation(textShader.program, "color"), color.r, color.g, color.b, color.a);
        glActiveTexture(GL_TEXTURE0);
        glBindVertexArray(vao);

        // iterate through all characters
        for (string::const_iterator c = text.begin(); c != text.end(); c++)
        {
            Character ch = characters[*c];

            float xpos = (pos.x + xOffset + ch.bearing.x * scale);
            float ypos = (pos.y - (ch.size.y - ch.bearing.y) * scale);

            float w = ch.size.x * scale;
            float h = ch.size.y * scale;
            // render glyph texture over quad
            glBindTexture(GL_TEXTURE_2D, ch.textureID);
            glUniform2f(glGetUniformLocation(textShader.program, "position"), xpos, ypos);
            glUniform2f(glGetUniformLocation(textShader.program, "scale"), w, h);
            // render quad
            glDrawArrays(GL_TRIANGLES, 0, 6);
            // now advance cursors for next glyph (note that advance is number of 1/64 pixels)
            xOffset += (ch.advance >> 6) * scale; // bitshift by 6 to get value in pixels (2^6 = 64)
        }
        glBindVertexArray(0);
        glBindTexture(GL_TEXTURE_2D, 0);
    }

    /*void RenderRotated(string text, vec2 pos, float rotation, float scale, vec4 color) // FIX THIS TO ROTATE AROUND CENTER
    {
        scale /= minimumSize;
        float xOffset = 0;
        // activate corresponding render state	
        glUseProgram(rotatedTextShader);
        glUniform1f(glGetUniformLocation(rotatedTextShader, "rotation"), rotation);
        glUniform4f(glGetUniformLocation(rotatedTextShader, "textColor"), color.r / 255.0f, color.g / 255.0f, color.b / 255.0f, color.a / 255.0f);
        glActiveTexture(GL_TEXTURE0);
        glBindVertexArray(vao);

        // iterate through all characters
        for (string::const_iterator c = text.begin(); c != text.end(); c++)
        {
            Character ch = characters[*c];

            float xpos = (pos.x + xOffset + ch.bearing.x * scale) / ScrWidth();
            float ypos = (pos.y - (ch.size.y - ch.bearing.y) * scale) / ScrHeight();

            float w = ch.size.x * scale / ScrWidth();
            float h = ch.size.y * scale / ScrHeight();
            // render glyph texture over quad
            glBindTexture(GL_TEXTURE_2D, ch.textureID);
            glUniform2f(glGetUniformLocation(rotatedTextShader, "position"), xpos, ypos);
            glUniform2f(glGetUniformLocation(rotatedTextShader, "scale"), w, h);
            // render quad
            glDrawArrays(GL_TRIANGLES, 0, 6);
            // now advance cursors for next glyph (note that advance is number of 1/64 pixels)
            xOffset += (ch.advance >> 6) * scale; // bitshift by 6 to get value in pixels (2^6 = 64)
        }
        glBindVertexArray(0);
        glBindTexture(GL_TEXTURE_2D, 0);
    }*/
};

vector<Font*> Font::fonts{};
FT_Library Font::ft;

Font font("PixeloidSans-JR6qo", 128);

GENERIC_SYSTEM(readFonts, ReadFonts, After, Start)
{
    if (FT_Init_FreeType(&Font::ft))
        cout << "ERROR::FREETYPE: Could not init FreeType Library\n";

    for (Font* font : Font::fonts)
    {
        ifstream file("Export/Fonts/" + font->fileLocation + ".ttf", ios::binary);
        vector<unsigned char> buffer(istreambuf_iterator<char>(file), {});

        font->Read(buffer.data(), buffer.size());

        file.close();
    }
}


NewComponent(NumberRenderer)
{
public:
    float size;
    uint number;
    byte renderLayer;

    NumberRenderer(float size, uint number = 0, byte renderLayer = RenderLayer::DEFAULT) :
        size(size), number(number), renderLayer(renderLayer) { SET_HASH; }

    string Text()
    {
        int remainingNumber = number;
        string text = string(static_cast<int>(ceil(log10f(max(1u, number)))), ' ');
        for (int i = text.size() - 1; i >= 0; i--, remainingNumber /= 10)
            text[i] = '0' + remainingNumber % 10;
        return text;
    }
};