#include "object3D.h"

void Object3D::draw(const glm::mat4 &pvMatrix)
{
     glUseProgram(_shaderProgram);
        
    // Full 3D transformation (X, Y, and Z)
    glm::mat4 model = glm::translate(glm::mat4(1.0f), _position);
        
    // Add rotation sandbox here if you want:
    // model = glm::rotate(model, (float)glfwGetTime(), glm::vec3(0, 1, 0));

    glm::mat4 mvp = pvMatrix * model;

    glUniformMatrix4fv(glGetUniformLocation(_shaderProgram, "uMVP"), 1, GL_FALSE, glm::value_ptr(mvp));
    glUniform4f(glGetUniformLocation(_shaderProgram, "uColor"), _color.r, _color.g, _color.b, _color.a);

    glBindVertexArray(_VAO);
    // 3D objects usually have 36 indices (12 triangles)
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0); 
    glBindVertexArray(0);
}