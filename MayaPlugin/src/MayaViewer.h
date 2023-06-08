#ifndef MayaViewer_H_
#define MayaViewer_H_

#include "gameplay.h"
#include <string>


using namespace gameplay;

/**
 * Main game class.
 */
class MayaViewer: public Game
{
public:

    /**
     * Constructor.
     */
    MayaViewer();

    /**
     * @see Game::keyEvent
     */
	void keyEvent(Keyboard::KeyEvent evt, int key);
	
    /**
     * @see Game::touchEvent
     */
    void touchEvent(Touch::TouchEvent evt, int x, int y, unsigned int contactIndex);

	// mouse events
	bool mouseEvent(Mouse::MouseEvent evt, int x, int y, int wheelDelta);


protected:

    /**
     * @see Game::initialize
     */
    void initialize();

    /**
     * @see Game::finalize
     */
    void finalize();

    /**
     * @see Game::update
     */
    void update(float elapsedTime);

    /**
     * @see Game::render
     */
    void render(float elapsedTime);

private:

    /**
     * Draws the scene each frame.
     */
    bool drawScene(Node* node);


	Mesh* createCubeMesh(float size = 1.0f);
	Material* createMaterial();

    Node* CreatePointLight(std::string name, Vector3 position = { 0, 1, 5 }, Vector3 color = { 1.0f, 1.0f, 1.0f }, float range = 20);
    //Node* CreateCamera(std::string name, Vector3 position = { 0, 0, 20 }, float fov = 45.0f, float znear = 1.0f, float zsfar = 100.0f);

    Scene* _scene;
    bool _wireframe;


};

#endif
