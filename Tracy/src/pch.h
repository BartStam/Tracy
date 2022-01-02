#pragma once

#define NOMINMAX
#define GLFW_INCLUDE_NONE
#define _USE_MATH_DEFINES

#include <fstream>
#include <sstream>
#include <iostream>
#include <iomanip>
#include <limits>

#include <vector>
#include <tuple>

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <stb_image.h>

#include <glm/glm.hpp>
#include <glm/gtx/string_cast.hpp>
#include <glm/gtx/rotate_vector.hpp>
#include <glm/gtc/constants.hpp>

#include "shader.h"
#include "math/ray.h"
#include "camera.h"
#include "rendercore.h"
