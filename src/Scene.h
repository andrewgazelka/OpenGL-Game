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
    Model &keyModel;
    TexturedModel &floorModel;
//    Model &keyModel;
    Model &doorModel;
};


struct SceneKey {
    size_t id;
    glm::vec3 location;
};

class Scene {
private:
    TextureData textures;
    const Map& map;
    unsigned int shaderProgram;
    GLint modelParam;
    GLint textureIdParam;
    GLint colorParam;
    glm::mat4 model;
    std::vector<SceneKey> keys;

public:
    Scene(const TextureData &data, const Map &map, unsigned int shaderProgram) : textures(data), map(map), shaderProgram(shaderProgram) {
        modelParam = glGetUniformLocation(shaderProgram, "model");
        textureIdParam = glGetUniformLocation(shaderProgram, "texID");
        colorParam = glGetUniformLocation(shaderProgram, "inColor");
        model = glm::mat4(1);

        for (int x = 0; x < map.width; ++x) {
            for (int y = 0; y < map.height; ++y) {
                auto element = map.GetElement(x, y);
                if (element.tag == Tag::KEY) {
                    glm::vec3 location(x, y, -.25);
                    SceneKey key = {
                            .id = element.value.key.id,
                            .location = location
                    };
                    keys.push_back(key);
                }
            }
        }
    }

    void ResetModel() {
        model = glm::mat4(1);
    }

    glm::vec3 GetStartPosition() {

        for (int x = 0; x < map.width; ++x) {
            for (int y = 0; y < map.height; ++y) {
                auto element = map.GetElement(x, y);
                if (element.tag == Tag::START) {
                    glm::vec3 res(x, y, 0.0);
                    return res;
                }
            }
        }
        throw std::logic_error("no start position");
    }

    bool IsCollision(float x, float y) {

        if (x < -0.5 || y < -0.5) return false;

        int maxX = map.width - 1;
        int maxY = map.height - 1;

        if (x > maxX + 0.5 || y > maxY + 0.5)return false;

        int iX = (int) std::round(x);
        int iY = (int) std::round(y);

        Element element = map.GetElement(iX, iY);

        switch (element.tag) {
            case Tag::START:
            case Tag::FINISH:
            case Tag::KEY:
            case Tag::EMPTY:
                return false;
            case Tag::WALL:
            case Tag::DOOR: // TODO: door optional
                return true;
        }

    }


    void Draw() {
        for (int x = 0; x < map.width; ++x) {
            for (int y = 0; y < map.height; ++y) {
                auto element = map.GetElement(x, y);

                auto fx = (float) x;
                auto fy = (float) y;

                switch (element.tag) {
                    case Tag::FINISH:
                    case Tag::START:
                    case Tag::EMPTY:
                    case Tag::KEY: // keys are dynamic—we draw them elsewhere
                        // no special drawing
                        break;
                    case Tag::DOOR:
                        Draw(fx, fy, 0.0f, textures.doorModel, (float) element.value.door.id / 10.0f, 0.0f, 0.0f);
                        break;
                    case Tag::WALL:
                        Draw(fx, fy, 0.0f, textures.wallModel);
                        break;
                }
                Draw(fx, fy, -1.0f, textures.floorModel);
            }
        }

        for(const auto& key: keys){
            auto id = key.id;
            Draw(key.location[0], key.location[1], key.location[2], textures.keyModel, (float) id / 10.0f, 0.0f, 0.0f, 0.5);
        }
    }

private:

    void Draw(float x, float y, float z, const Model &model, float r, float g, float b, float scale = 1.0) {
        SetColor(r, g, b);
        SetTranslation(x, y, z);
        SetScale(scale);
        SendTransformations();
        model.draw();
        ResetModel();
    }

    void Draw(float x, float y, float z, const TexturedModel &texturedModel, float scale = 1.0) {
        SetTexture(texturedModel.textureId);
        SetTranslation(x, y, z);
        SetScale(scale);
        SendTransformations();
        texturedModel.model.draw();
        ResetModel();
    }

    void SendTransformations() {
        glUniformMatrix4fv(modelParam, 1, GL_FALSE, glm::value_ptr(model));
    }

    void SetScale(float x) {
        model = glm::scale(model, glm::vec3(x, x, x));
    }

    void SetTranslation(float x, float y, float z = 0) {
        model = glm::translate(model, glm::vec3(x, y, z));
    }

    void SetTexture(int id) {
        glUniform1i(textureIdParam, id);
    }

    void SetColor(float r, float g, float b) {
        glm::vec3 colVec(r, g, b);
        glUniform3fv(colorParam, 1, glm::value_ptr(colVec));
        SetTexture(-1);
    }

};

