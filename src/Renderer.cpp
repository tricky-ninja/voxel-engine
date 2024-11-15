#include "Renderer.h"

#include "Logger.h"
#include <fstream>
#include <sstream>
#include <stb_image.h>
#include <sys/stat.h>
#include <cmath>

long long get_timestamp(std::string filepath)
{
    struct stat fileStat = {};
    stat(filepath.c_str(), &fileStat);
    return fileStat.st_mtime;
}

std::string read_file(const char* filepath)
{
    std::ifstream file(filepath);
    if (!file.is_open()) {
        Log_warn << "Could not open file : " << filepath << "\n";
        return std::string("");
    }

    std::stringstream buffer;
    buffer << file.rdbuf();

    std::string fileContents = buffer.str();

    return fileContents;
}

#pragma region Shaders

bool Shader::load_from_file(const char* vertexShaderPath, const char* fragmentShaderPath)
{

    std::string vertexShaderContents = read_file(vertexShaderPath);
    std::string fragmentShaderContents = read_file(fragmentShaderPath);

    if (vertexShaderContents == "" || fragmentShaderContents == "")
    {
        Log_warn << "Shader file(s) are empty " << vertexShaderPath << " " << fragmentShaderPath << "\n";
        return false;
    }

    const char* vertexShaderSource = vertexShaderContents.c_str();
    const char* fragmentShaderSource = fragmentShaderContents.c_str();

    int success;
    char errorBuffer[512];

    unsigned int vertexShaderID = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShaderID, 1, &vertexShaderSource, NULL);
    glCompileShader(vertexShaderID);

    glGetShaderiv(vertexShaderID, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(vertexShaderID, 512, NULL, errorBuffer);
        Log_warn << "Vertex shader " << vertexShaderPath << " failed to compile\n" << errorBuffer << "\n";
        return false;
    }

    unsigned int fragmentShaderID = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShaderID, 1, &fragmentShaderSource, NULL);
    glCompileShader(fragmentShaderID);

    glGetShaderiv(fragmentShaderID, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(fragmentShaderID, 512, NULL, errorBuffer);
        Log_warn << "Fragment shader " << fragmentShaderPath << " failed to compile\n" << errorBuffer << "\n";
        return false;
    }

    id = glCreateProgram();
    glAttachShader(id, vertexShaderID);
    glAttachShader(id, fragmentShaderID);
    glLinkProgram(id);

    glGetProgramiv(id, GL_LINK_STATUS, &success);
    if (!success)
    {
        glGetProgramInfoLog(id, 512, NULL, errorBuffer);
        Log_warn << "Failed to link shader " << vertexShaderPath << " and " << fragmentShaderPath << "\n" << errorBuffer << "\n";
        return false;
    }

    this->vertexFilepath = vertexShaderPath;
    this->fragmentFilepath = fragmentShaderPath;

    this->vertexTimestamp = get_timestamp(this->vertexFilepath);
    this->fragmentTimestamp = get_timestamp(this->fragmentFilepath);

    glDeleteShader(vertexShaderID);
    glDeleteShader(fragmentShaderID);

    return true;
}

void Shader::bind() const
{
    glUseProgram(id);
}

void Shader::unbind() const
{
    glUseProgram(0);
}

void Shader::clear()
{
    glDeleteProgram(id);
}

bool Shader::set_bool(const std::string& name, bool value) const
{
    int location = glGetUniformLocation(id, name.c_str());
    if (location == -1) return false;
    glUniform1i(location, (int)value);
    return true;
}

bool Shader::set_int(const std::string& name, int value) const
{
    int location = glGetUniformLocation(id, name.c_str());
    if (location == -1) return false;
    glUniform1i(location, value);
    return true;
}

bool Shader::set_unsigned(const std::string& name, unsigned value) const
{
    int location = glGetUniformLocation(id, name.c_str());
    if (location == -1) return false;
    glUniform1ui(location, value);
    return true;
}

bool Shader::set_float(const std::string& name, float value) const
{
    int location = glGetUniformLocation(id, name.c_str());
    if (location == -1) return false;
    glUniform1f(location, value);
    return true;
}

bool Shader::set_vec2(const std::string& name, const float* values) const
{
    int location = glGetUniformLocation(id, name.c_str());
    if (location == -1) return false;
    glUniform2fv(location, 1, values);
    return true;
}

bool Shader::set_vec3(const std::string& name, const float* values) const
{
    int location = glGetUniformLocation(id, name.c_str());
    if (location == -1) return false;
    glUniform3fv(location, 1, values);
    return true;
}

bool Shader::set_vec4(const std::string& name, const float* values) const
{
    int location = glGetUniformLocation(id, name.c_str());
    if (location == -1) return false;
    glUniform4fv(location, 1, values);
    return true;
}

bool Shader::set_mat2(const std::string& name, const float* value) const
{
    int location = glGetUniformLocation(id, name.c_str());
    if (location == -1) return false;
    glUniformMatrix2fv(location, 1, GL_FALSE, value);
    return true;
}

bool Shader::set_mat3(const std::string& name, const float* value) const
{
    int location = glGetUniformLocation(id, name.c_str());
    if (location == -1) return false;
    glUniformMatrix3fv(location, 1, GL_FALSE, value);
    return true;
}

bool Shader::set_mat4(const std::string& name, const float* value) const
{
    int location = glGetUniformLocation(id, name.c_str());
    if (location == -1) return false;
    glUniformMatrix4fv(location, 1, GL_FALSE, value);
    return true;
}

void Shader::hot_reload()
{
    long long currentVertexTime = get_timestamp(vertexFilepath);
    long long currentFragmentTime = get_timestamp(fragmentFilepath);

    if (currentVertexTime > vertexTimestamp || currentFragmentTime > fragmentTimestamp)
    {
        Shader tmp;
        if (!tmp.load_from_file(vertexFilepath.c_str(), fragmentFilepath.c_str()))
        {
            perm_assert_msg(false, "Error recompiling shader. Fix the error and retry");
            if (!tmp.load_from_file(vertexFilepath.c_str(), fragmentFilepath.c_str())) perm_assert_msg(false, "Error not fixed, program may crash");
        }
        unbind();
        clear();
        this->id = tmp.id;
        this->fragmentTimestamp = tmp.fragmentTimestamp;
        this->vertexTimestamp = tmp.vertexTimestamp;
    }
}

#pragma endregion

#pragma region Textures

bool Texture::load_from_file(const char* filepath, unsigned int wrap, unsigned int filtering)
{
    perm_assert((wrap == GL_REPEAT || wrap == GL_MIRRORED_REPEAT || wrap == GL_CLAMP_TO_EDGE) && (filtering == GL_NEAREST || filtering == GL_LINEAR));

    stbi_set_flip_vertically_on_load(true);
    glGenTextures(1, &id);

    unsigned char* data = stbi_load(filepath, &width, &height, &bitsPerPixel, 0);
    if (!data)
    {
        Log_warn << "ERORR: Failed to load texture " << filepath << "\n";
        return false;
    }

    this->filepath = filepath;
    this->timestamp = get_timestamp(this->filepath);

    glBindTexture(GL_TEXTURE_2D, id);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrap);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrap);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, filtering + 0x102);	// +0x102 selects the apropriate mipmap level
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, filtering);

    unsigned int format = GL_RGBA;
    if (bitsPerPixel == 4) format = GL_RGBA;
    if (bitsPerPixel == 3) format = GL_RGB;
    if (bitsPerPixel == 1) format = GL_RED;

    glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 4);
    glGenerateMipmap(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, 0);
    stbi_image_free(data);
    return true;
}

void Texture::hot_reload()
{
    long long currentTimestamp = get_timestamp(this->filepath);
    if (currentTimestamp > timestamp)
    {
        unbind();
        clear();
       load_from_file(this->filepath.c_str());
    }
}

void Texture::bind(int slot)
{
    this->slot = slot;
    glActiveTexture(GL_TEXTURE0 + slot);
    glBindTexture(GL_TEXTURE_2D, id);
}

void Texture::unbind()
{
    glActiveTexture(GL_TEXTURE0 + this->slot);
    glBindTexture(GL_TEXTURE_2D, 0);
}

void Texture::clear()
{
    glDeleteTextures(1, &id);
}

#pragma endregion

#pragma region Camera

int sign(float num)
{
    if (num == 0.f) return 0;
    return num / abs(num);
}

void Camera::move(CAMERA_MOVEMENT moveDirection, float deltaTime)
{
    if (moveDirection == FORWARD) pos += glm::normalize(glm::vec3(direction.x, 0, direction.z)) * moveSpeed * deltaTime;
    if (moveDirection == BACKWARD) pos -= glm::normalize(glm::vec3(direction.x, 0, direction.z)) * moveSpeed * deltaTime;
    if (moveDirection == RIGHT) pos += glm::normalize(glm::cross(direction, up)) * moveSpeed * deltaTime;
    if (moveDirection == LEFT) pos -= glm::normalize(glm::cross(direction, up)) * moveSpeed * deltaTime;
    if (moveDirection == UP) pos += glm::normalize(up) * moveSpeed * deltaTime;
    if (moveDirection == DOWN) pos -= glm::normalize(up) * moveSpeed * deltaTime;
    update_vectors();
}

void Camera::look_at(glm::vec3 target)
{
    direction = glm::normalize(target);
}

glm::mat4 Camera::get_view() const
{
    return glm::lookAt(pos, pos + direction, up);
}

void Camera::update_vectors()
{
    glm::vec3 front;
    front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    front.y = sin(glm::radians(pitch));
    front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
    direction = glm::normalize(front);
}

std::string Camera::get_coords_as_string()
{
    return "X: " + std::to_string(pos.x) + "\nY: " + std::to_string(pos.y) + "\nZ: " + std::to_string(pos.z);
}

void Camera::turn(float xoffset, float yoffset, bool constraintPitch)
{
    xoffset *= sensitivity;
    yoffset *= sensitivity;

    yaw += xoffset;
    pitch += yoffset;

    if (constraintPitch)
    {
        if (pitch > 89.0f)
            pitch = 89.0f;
        if (pitch < -89.0f)
            pitch = -89.0f;
    }

    update_vectors();
}

void Camera::zoom(float yoffset)
{
    fov -= (float)yoffset;
    if (fov < 1.0f)
        fov = 1.0f;
    if (fov > 45.0f)
        fov = 45.0f;
}


#pragma endregion

void draw_mesh(const Mesh& mesh, const Shader &shader, const Camera& camera, glm::mat4 model)
{

    glm::mat4 view = glm::mat4(1.0f);
    view = camera.get_view();

    glm::mat4 projection = glm::mat4(1.0f);
    projection = glm::perspective(glm::radians(camera.fov), 800.0f / 600.0f, 0.1f, 100000.0f);

    glm::mat3 normalMat = glm::mat3(1.f);
    normalMat = glm::transpose(glm::inverse(view * model));

    glm::vec3 up = glm::vec3(-0.5, 1, 0.5);
    up = glm::normalize(up);
    up = normalMat * up;

    glBindVertexArray(mesh.VAO);
    shader.bind();
    shader.set_vec3("up", glm::value_ptr(up));
    shader.set_mat3("normalMatrix", glm::value_ptr(normalMat));
    shader.set_mat4("model", glm::value_ptr(model));
    shader.set_mat4("view", glm::value_ptr(view));
    shader.set_mat4("projection", glm::value_ptr(projection));
    glDrawArrays(GL_TRIANGLES, 0, mesh.vertices.size());
}

void Mesh::setup()
{
    if (vertices.size() == 0)
    {
        Log_warn << "Tried to initialise mesh without vertices\n";
        return;
    }

    if (VAO != 0)
    {
        glDeleteVertexArrays(1, &VAO);
    }

    if (VBO != 0) glDeleteBuffers(1, &VBO);

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);

    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(PackedVertexData), &vertices[0], GL_STATIC_DRAW);

    glEnableVertexAttribArray(0); // position
    glVertexAttribPointer(0,3, GL_UNSIGNED_BYTE, GL_FALSE, sizeof(PackedVertexData), (void*)offsetof(PackedVertexData, x));

    glEnableVertexAttribArray(1); // cube face
    glVertexAttribPointer(1, 2, GL_UNSIGNED_BYTE, GL_FALSE, sizeof(PackedVertexData), (void*)offsetof(PackedVertexData, face));

    glEnableVertexAttribArray(2); // texture coords
    glVertexAttribPointer(2, 1, GL_UNSIGNED_BYTE, GL_FALSE, sizeof(PackedVertexData), (void*)offsetof(PackedVertexData, uv));

    glEnableVertexAttribArray(3); // texture id (in the atlas)
    glVertexAttribPointer(3, 1, GL_UNSIGNED_BYTE, GL_FALSE, sizeof(PackedVertexData), (void*)offsetof(PackedVertexData, textureID));

}

void set_position(PackedVertexData &vertexData, glm::vec3 position)
{
    unsigned int xPos = position.x;
    unsigned int yPos = position.y;
    unsigned int zPos = position.z;

    vertexData.x = xPos;
    vertexData.y = yPos;
    vertexData.z = zPos;
}

glm::vec3 get_position(PackedVertexData &vertexData)
{
    return glm::vec3(vertexData.x, vertexData.y, vertexData.z);
}

void set_normal(PackedVertexData &vertexData, glm::vec3 normal)
{
    glm::vec3 absNormal = { abs(normal.x), abs(normal.y), abs(normal.z) };

    perm_assert_msg((absNormal == glm::vec3(1,0,0)) || (absNormal == glm::vec3(0, 1, 0)) || (absNormal == glm::vec3(0, 0, 1)), "Invalid vertex normals");

    if (normal.x == -1)
    {
        vertexData.face = (FACE_LEFT);
        return;
    }

    if (normal.x == 1)
    {
        vertexData.face = (FACE_RIGHT);
        return;
    }

    if (normal.y == -1)
    {
        vertexData.face = (FACE_BOTTOM);
        return;
    }

    if (normal.y == 1)
    {
        vertexData.face = (FACE_TOP);
        return;
    }

    if (normal.z == -1)
    {
        vertexData.face = (FACE_BACK);
        return;
    }

    if (normal.z == 1)
    {
        vertexData.face = (FACE_FRONT);
        return;
    }
}

glm::vec3 get_normal(PackedVertexData &vertexData)
{
    int normal;
    normal = vertexData.face;

    switch (normal)
    {
    case FACE_LEFT:
        return glm::vec3(-1, 0, 0);
    case FACE_RIGHT:
        return glm::vec3(1, 0, 0);
    case FACE_BOTTOM:
        return glm::vec3(0, -1, 0);
    case FACE_TOP:
        return glm::vec3(0, 1, 0);
    case FACE_BACK:
        return glm::vec3(0, 0, -1);
    case FACE_FRONT:
        return glm::vec3(0, 0, 1);
    }
    return glm::vec3();
}

void set_AO(PackedVertexData &vertexData, uint8_t ao)
{
    vertexData.ao = ao;
}

uint8_t get_AO(PackedVertexData &vertexData)
{
    return vertexData.ao;
}

Framebuffer::Framebuffer(unsigned width, unsigned height)
{
    glGenFramebuffers(1, &id);

    glBindFramebuffer(GL_FRAMEBUFFER, id);

    glGenTextures(1, &textureColorBuffer);
    glBindTexture(GL_TEXTURE_2D, textureColorBuffer);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glBindTexture(GL_TEXTURE_2D, 0);

    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, textureColorBuffer, 0);

    glGenRenderbuffers(1, &depthStencilBuffer);
    glBindRenderbuffer(GL_RENDERBUFFER, depthStencilBuffer);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT32, width, height);
    glBindRenderbuffer(GL_RENDERBUFFER, 0);

    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthStencilBuffer);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
    {
        perm_assert_msg(false, "Framebuffer is not complete!\n");
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Framebuffer::bind()
{
    glBindFramebuffer(GL_FRAMEBUFFER, id);
}

void Framebuffer::unbind()
{
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Framebuffer::clear()
{
    glDeleteFramebuffers(1, &id);
}
