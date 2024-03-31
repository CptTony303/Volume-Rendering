#ifndef VOLUME_GUI_H
#define VOLUME_GUI_H

#include <imgui.h>
#include <vector>
#include <glm/glm.hpp>

class VolumeGUI {
public:
	VolumeGUI();
	void renderGUI();
	std::vector<float> getTransferFunction(int type);

	int methode = 0;
	float stepSize = 0.01f;
	int samplesPerFrame = 5;
	float brightness = 10.f;
	bool reset_accumulation_frames;
private:
	void renderMethodeTab();
	void transferFunctionTab();
	void aboutTab();

	void functionEditor(int type);
	int getPointClicked(ImVec2 mouse_pos_in_canvas, ImVector<ImVec2>* points, float detecting_radius);
	float distance(ImVec2 a, ImVec2 b);
	void loadVector(ImVector<ImVec2>* points, std::vector<glm::vec2> glm_points, ImVec2 canvas_sz, ImVec2 origin);
	void safeVector(ImVector<ImVec2>* points, std::vector<glm::vec2>* glm_points, ImVec2 canvas_sz, ImVec2 origin);
	int space_for_new_point(ImVector<ImVec2>* points, ImVec2 mouse_pos, float space_between_points);

	std::vector<glm::vec2> trans_func_points_color = { glm::vec2(0.f,0.f),glm::vec2(0.5f,0.5f),glm::vec2(1.f,1.f) };
	std::vector<glm::vec2> trans_func_points_density = { glm::vec2(0.f,0.f),glm::vec2(0.5f,0.5f),glm::vec2(1.f,1.f) };
	bool is_dragging_point_color = false, is_dragging_point_density = false;
	int dragging_point = -1;
	enum FuncEditorType { COLOR, DENSITY };
};

#endif // !VOLUME_GUI_H
