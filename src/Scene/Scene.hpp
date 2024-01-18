// Singleton scene class
#include <memory>

class Scene {
   public:
    Scene(Scene &other) = delete;
    void operator=(const Scene &) = delete;

    static std::shared_ptr<Scene> GetInstance();

   private:
    static std::shared_ptr<Scene> instance;
    Scene();
};