#pragma once

#include <repr/Map.h>

#include <utility>
#include "utils.h"

struct TexturedModel {
    Model &model;
    unsigned int textureId;
};

struct TextureData {
    TexturedModel &wallModel;
//    TexturedModel &floorModel;
//    Model &keyModel;
//    Model &doorModel;
};

class Scene {
private:
    TextureData textures;
    Map map;
    unsigned int shaderProgram;
    GLint modelParam;
    GLint textureIdParam;
    GLint colorParam;

public:
    Scene(const TextureData &data, Map map, unsigned int shaderProgram) : textures(data), map(std::move(map)), shaderProgram(shaderProgram) {
        modelParam = glGetUniformLocation(shaderProgram, "model");
        textureIdParam = glGetUniformLocation(shaderProgram, "texID");
        colorParam = glGetUniformLocation(shaderProgram, "inColor");
    }

    void Draw() const {
        for (int x = 0; x < map.width; ++x) {
            for (int y = 0; y < map.height; ++y) {
                auto element = map.GetElement(x, y);
                switch (element.tag) {
                    case Tag::FINISH:
                    case Tag::START:
                    case Tag::EMPTY:
                        // no special drawing
                        break;
                    case Tag::DOOR:
                        break;
                    case Tag::KEY:
                        break;
                    case Tag::WALL:
                        Draw(x, y , textures.wallModel);
                        break;
                }
            }
        }
    }

private:

    void Draw(int x, int y, const Model &model, float r, float g, float b) const {
        SetColor(r, g, b);
        SetTranslation(x, y);
        model.draw();
    }

    void Draw(int x, int y, const TexturedModel &texturedModel) const {
        SetTexture(texturedModel.textureId);
        SetTranslation(x, y);
        texturedModel.model.draw();
    }

    void SetTranslation(int x, int y) const {
        glm::mat4 model(1);
        model = glm::translate(model, glm::vec3(x, y, 0));
        glUniformMatrix4fv(modelParam, 1, GL_FALSE, glm::value_ptr(model));
    }

    void SetTexture(int id) const {
        glUniform1i(textureIdParam, id);
    }

    void SetColor(float r, float g, float b) const {
        glm::vec3 colVec(r, g, b);
        glUniform3fv(colorParam, 1, glm::value_ptr(colVec));
        SetTexture(-1);
    }

};

