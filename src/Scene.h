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
//    TexturedModel &floorModel;
//    Model &keyModel;
    Model &doorModel;
};

class Scene {
private:
    TextureData textures;
    Map map;
    unsigned int shaderProgram;
    GLint modelParam;
    GLint textureIdParam;
    GLint colorParam;
    glm::mat4 model;

public:
    Scene(const TextureData &data, Map map, unsigned int shaderProgram) : textures(data), map(std::move(map)), shaderProgram(shaderProgram) {
        modelParam = glGetUniformLocation(shaderProgram, "model");
        textureIdParam = glGetUniformLocation(shaderProgram, "texID");
        colorParam = glGetUniformLocation(shaderProgram, "inColor");
        model = glm::mat4(1);
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
                switch (element.tag) {
                    case Tag::FINISH:
                    case Tag::START:
                    case Tag::EMPTY:
                        // no special drawing
                        break;
                    case Tag::DOOR:
                        Draw(x, y, textures.doorModel, (float) element.value.door.id / 10.0f, 0.0f, 0.0f);
                        break;
                    case Tag::KEY:
                        Draw(x, y, textures.keyModel, (float) element.value.door.id / 10.0f, 0.0f, 0.0f, 0.5);
                        break;
                    case Tag::WALL:
                        Draw(x, y, textures.wallModel);
                        break;
                }
            }
        }
    }

private:

    void Draw(int x, int y, const Model &model, float r, float g, float b, float scale = 1.0) {
        SetColor(r, g, b);
        SetTranslation(x, y);
        SetScale(scale);
        SendTransformations();
        model.draw();
        ResetModel();
    }

    void Draw(int x, int y, const TexturedModel &texturedModel, float scale = 1.0) {
        SetTexture(texturedModel.textureId);
        SetTranslation(x, y);
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

    void SetTranslation(int x, int y) {
        model = glm::translate(model, glm::vec3(x, y, 0));
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

