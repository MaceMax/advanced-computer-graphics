
/**
 * @brief State
A class containing all state information. Shall include an apply method to apply all contained state. Required state data is:

    Lighting (enable/disable, global and for each light)
    CullFace (enable/disable culling of backfacing faces) (See GL_CULL_FACE)
    Material - one material can be applied to the current state
    Texture - one or more textures can be applied to the current state.
    A State should also consist of Shader (Vertex/Fragment/Geometry) and Program. A Program also has a number of Uniforms and Attributes

 *
 */
class State {
   public:
    State() = default;
    ~State() = default;

    void Apply();

   private:
    bool lightingEnabled;
    bool cullFaceEnabled;
    // Material material;
    // Texture texture;
    // Shader shader;
};