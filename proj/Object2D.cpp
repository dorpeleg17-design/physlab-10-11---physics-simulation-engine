#include "Object2D.h"
void Object2D::draw(const glm::mat4 &pvMatrix)
{
    glUseProgram(_shaderProgram);
        
    // Simple 2D translation
    glm::mat4 model = glm::translate(glm::mat4(1.0f), glm::vec3(_position.x, _position.y, 0.0f));
    glm::mat4 mvp = pvMatrix * model;

    glUniformMatrix4fv(glGetUniformLocation(_shaderProgram, "uMVP"), 1, GL_FALSE, glm::value_ptr(mvp));
    glUniform4f(glGetUniformLocation(_shaderProgram, "uColor"), _color.r, _color.g, _color.b, _color.a);

    glBindVertexArray(_VAO);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
}