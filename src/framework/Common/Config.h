#pragma once
#include "Scene/Compoments/Camera.h"

class Config {
public:
    void  SaveConfig();
    Config static GetInstance();
    void  CameraFromConfig(Camera & camera);
    void  CameraToConfig(const Camera & camera);
protected:
    Config();
    static Config* instance;
    class Impl;
    Impl* impl{nullptr};
};

