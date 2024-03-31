#include <live-application/GUI/volumeGUI.h>

#include <imgui.h>
#include <corecrt_math.h>
#include <iostream>
#include <vector>


VolumeGUI::VolumeGUI()
{
}

void VolumeGUI::renderGUI()
{
	ImGui::Begin("Volume Renderer Settings");
	if (ImGui::BeginTabBar("##TabBar"))
	{
		renderMethodeTab();
		transferFunctionTab();
		aboutTab();

		ImGui::EndTabBar();
	}
	ImGui::End();
}

std::vector<float> VolumeGUI::getTransferFunction(int type)
{
	std::vector<glm::vec2> points;
	std::vector<float> transferFunction;
	switch (type) {
	case 0:
		points = trans_func_points_color;
		break;
	case 1:
		points = trans_func_points_density;
		break;
	}
	for each (glm::vec2 p in points)
	{
		transferFunction.push_back(float(p.x));
		transferFunction.push_back(float(p.y));
	}
	return transferFunction;
}

void VolumeGUI::renderMethodeTab()
{
	if (ImGui::BeginTabItem("render methode"))
	{
		ImGui::Text("Select your render methode:");
		ImGui::RadioButton("Ray Marching", &methode, 0); ImGui::SameLine();
		ImGui::RadioButton("Monte Carlo Sampling", &methode, 1);
		ImGui::Text("\nJust for debug purposes:");
		ImGui::RadioButton("Just rng demo", &methode, 2); ImGui::SameLine();
		ImGui::RadioButton("Transfer Function visualization", &methode, 3);
		ImGui::RadioButton("colored cube", &methode, 4);
		if (methode == 0) {
			ImGui::Text("\nSet the step size for the ray march:");
			ImGui::SliderFloat("stepSize", &stepSize, 0.001f, 0.02f, "%.5f");
		}
		if (methode == 1) {
			ImGui::Text("\nSet the brightness:");
			ImGui::SliderFloat("brightness", &brightness, 1.f, 100.f, "%.5f");
		}
		ImGui::EndTabItem();
	}
}

void VolumeGUI::transferFunctionTab()
{
	if (ImGui::BeginTabItem("transfer function"))
	{
		ImGui::Text("Mouse Left: drag to move points,\nMouse Right: Add new point or delete existing point.");

		functionEditor(0);
		functionEditor(1);
		ImGui::EndTabItem();
	}
}

void VolumeGUI::aboutTab()
{
	if (ImGui::BeginTabItem("about"))
	{
		ImGui::Text("This is a simple Volume renderer coded by Anton Hartmann.");
		ImGui::EndTabItem();
	}
}

void VolumeGUI::functionEditor(int type)
{

	//type: 0 for color, 1 for density
	bool is_dragging_point;
	float space_between_points = 20.f;
	float point_radius = space_between_points - 10;
	float detecting_radius = space_between_points - 5.f;
	float line_thickness = 2.f;

	switch (type) {
	case COLOR:
		is_dragging_point = is_dragging_point_color;
		ImGui::Text("Set the transfer function for the color");
		break;
	case DENSITY:
		is_dragging_point = is_dragging_point_density;
		ImGui::Text("Set the transfer function for the density");
		break;
	default:
		break;
	}


	// Typically you would use a BeginChild()/EndChild() pair to benefit from a clipping region + own scrolling.
	// Here we demonstrate that this can be replaced by simple offsetting + custom drawing + PushClipRect/PopClipRect() calls.
	// To use a child window instead we could use, e.g:
	//      ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));      // Disable padding
	//      ImGui::PushStyleColor(ImGuiCol_ChildBg, IM_COL32(50, 50, 50, 255));  // Set a background color
	//      ImGui::BeginChild("canvas", ImVec2(0.0f, 0.0f), ImGuiChildFlags_Border, ImGuiWindowFlags_NoMove);
	//      ImGui::PopStyleColor();
	//      ImGui::PopStyleVar();
	//      [...]
	//      ImGui::EndChild();

	// Using InvisibleButton() as a convenience 1) it will advance the layout cursor and 2) allows us to use IsItemHovered()/IsItemActive()
	ImVec2 canvas_p0 = ImGui::GetCursorScreenPos();      // ImDrawList API uses screen coordinates!
	ImVec2 canvas_sz = ImGui::GetContentRegionAvail();   // Resize canvas to what's available
	if (type == 0)
		canvas_sz.y /= 2;
	canvas_sz.y -= 2;
	if (canvas_sz.x < 50.0f) canvas_sz.x = 50.0f;
	if (canvas_sz.y < 50.0f) canvas_sz.y = 50.0f;
	ImVec2 canvas_p1 = ImVec2(canvas_p0.x + canvas_sz.x, canvas_p0.y + canvas_sz.y);

	ImVector<ImVec2> points;
	if (type == 0) {
		loadVector(&points, trans_func_points_color, canvas_sz, canvas_p0);
	}
	if (type == 1) {
		loadVector(&points, trans_func_points_density, canvas_sz, canvas_p0);
	}

	// Draw border and background color
	ImGuiIO& io = ImGui::GetIO();
	ImDrawList* draw_list = ImGui::GetWindowDrawList();
	if (type == 0) {
		ImU32 col_a = ImGui::GetColorU32(IM_COL32(255, 0, 0, 255));
		ImU32 col_b = ImGui::GetColorU32(IM_COL32(0, 255, 0, 255));
		ImU32 col_c = ImGui::GetColorU32(IM_COL32(0, 0, 255, 255));
		ImVec2 canvas_p1_1 = ImVec2(canvas_p0.x + canvas_sz.x, canvas_p0.y + canvas_sz.y / 2);
		ImVec2 canvas_p0_2 = ImVec2(canvas_p0.x, canvas_p0.y + canvas_sz.y / 2);
		draw_list->AddRectFilledMultiColor(canvas_p0, canvas_p1_1, col_a, col_a, col_b, col_b);
		draw_list->AddRectFilledMultiColor(canvas_p0_2, canvas_p1, col_b, col_b, col_c, col_c);
	}
	if (type == 1) {
		ImU32 col_a = ImGui::GetColorU32(IM_COL32(255, 255, 255, 255));
		ImU32 col_b = ImGui::GetColorU32(IM_COL32(0, 0, 0, 255));
		draw_list->AddRectFilledMultiColor(canvas_p0, canvas_p1, col_a, col_a, col_b, col_b);
	}
	draw_list->AddRect(canvas_p0, canvas_p1, IM_COL32(255, 255, 255, 255));


	// This will catch our interactions
	ImGui::InvisibleButton("canvas", canvas_sz, ImGuiButtonFlags_MouseButtonLeft | ImGuiButtonFlags_MouseButtonRight);
	const bool is_hovered = ImGui::IsItemHovered(); // Hovered
	const bool is_active = ImGui::IsItemActive();   // Held
	const ImVec2 mouse_pos(io.MousePos.x, io.MousePos.y);

	// Drag existing points
	if (is_hovered && !is_dragging_point && ImGui::IsMouseClicked(ImGuiMouseButton_Left)) {
		//TODO: check for point to move
		dragging_point = getPointClicked(mouse_pos, &points, detecting_radius);
		if (dragging_point >= 0) {
			is_dragging_point = true;
		}
	}
	if (is_dragging_point)
	{
		//update point position
		points[dragging_point].x = mouse_pos.x;
		//prevent point from leaving canvas
		if (mouse_pos.x < canvas_p0.x) {
			points[dragging_point].x = canvas_p0.x;
		}
		else if (mouse_pos.x > canvas_p1.x) {
			points[dragging_point].x = canvas_p1.x;
		}
		points[dragging_point].y = mouse_pos.y;
		if (mouse_pos.y < canvas_p0.y) {
			points[dragging_point].y = canvas_p0.y;
		}
		else if (mouse_pos.y > canvas_p1.y) {
			points[dragging_point].y = canvas_p1.y;
		}
		//prevent point.x from getting to close to points on left and right IF NOT EDGES
		if (dragging_point > 0 && dragging_point < points.Size - 1) {
			//check left point
			if (points[dragging_point].x < points[dragging_point - 1].x + space_between_points) {
				points[dragging_point].x = points[dragging_point - 1].x + space_between_points;
			}
			//check right point
			if (points[dragging_point].x > points[dragging_point + 1].x - space_between_points) {
				points[dragging_point].x = points[dragging_point + 1].x - space_between_points;
			}
		}
		//special case for point.x is 0 or 1 -- no change in point.x
		if (dragging_point == 0) {
			points[dragging_point].x = canvas_p0.x;
		}
		else if (dragging_point == points.Size - 1) {
			points[dragging_point].x = canvas_p1.x;
		}

		// set dragging to false if mouse released
		if (!ImGui::IsMouseDown(ImGuiMouseButton_Left)) {
			is_dragging_point = false;
		}
	}

	// add or remove points
	if (is_hovered && ImGui::IsMouseClicked(ImGuiMouseButton_Right))
	{
		//check for existing point
		int clicked = getPointClicked(mouse_pos, &points, detecting_radius);
		int index = -1;
		//if exists and not edge point -> delete
		if (clicked != -1 && clicked != 0 && clicked != points.Size-1){
			points.erase(&points[clicked]);
			reset_accumulation_frames = true;
		}
		//otherwise check for distance between points
		else if (clicked == -1){
			index = space_for_new_point(&points, mouse_pos, space_between_points);
		}
		//and add new point if distance is given
		if(index != -1){
			points.insert(&points[index], ImVec2(mouse_pos));
			reset_accumulation_frames = true;
		}
	}

	// Draw grid + all lines in the canvas
	draw_list->PushClipRect(canvas_p0, canvas_p1, true);
	//if (opt_enable_grid)
	//{
	//    const float GRID_STEP = 64.0f;
	//    for (float x = fmodf(scrolling.x, GRID_STEP); x < canvas_sz.x; x += GRID_STEP)
	//        draw_list->AddLine(ImVec2(canvas_p0.x + x, canvas_p0.y), ImVec2(canvas_p0.x + x, canvas_p1.y), IM_COL32(200, 200, 200, 40));
	//    for (float y = fmodf(scrolling.y, GRID_STEP); y < canvas_sz.y; y += GRID_STEP)
	//        draw_list->AddLine(ImVec2(canvas_p0.x, canvas_p0.y + y), ImVec2(canvas_p1.x, canvas_p0.y + y), IM_COL32(200, 200, 200, 40));
	//}
	ImU32 col_point;
	ImU32 col_line;
	if (type == 0) {
		col_point = ImGui::GetColorU32(IM_COL32(0, 0, 0, 255));
		col_line = ImGui::GetColorU32(IM_COL32(255, 255, 255, 255));
	}
	if (type == 1) {
		col_point = ImGui::GetColorU32(IM_COL32(255, 0, 0, 255));
		col_line = ImGui::GetColorU32(IM_COL32(0, 255, 0, 255));
	}
	for (int n = 1; n < points.Size; n++) {
		ImVec2 center = ImVec2(points[n].x, points[n].y);
		ImVec2 center_last = ImVec2(points[n - 1].x, points[n - 1].y);
		draw_list->AddLine(center_last, center, col_line, line_thickness);
		draw_list->AddCircleFilled(center_last, point_radius, col_point, 10);
	}
	if (points.Size > 0) {
		ImVec2 center = ImVec2(points[points.Size - 1].x, points[points.Size - 1].y);
		draw_list->AddCircleFilled(center, point_radius, col_point, 10);
	}
	draw_list->PopClipRect();
	if (is_dragging_point) {
		reset_accumulation_frames = true;
	}
	switch (type) {
	case COLOR:
		safeVector(&points, &trans_func_points_color, canvas_sz, canvas_p0);
		is_dragging_point_color = is_dragging_point;
		break;
	case DENSITY:
		safeVector(&points, &trans_func_points_density, canvas_sz, canvas_p0);
		is_dragging_point_density = is_dragging_point;
		break;
	default:
		break;
	}

}

float VolumeGUI::distance(ImVec2 a, ImVec2 b)
{
	return sqrt(powf(a.x - b.x, 2.f) + powf(a.y - b.y, 2.f));
}

void VolumeGUI::loadVector(ImVector<ImVec2>* points, std::vector<glm::vec2> glm_points, ImVec2 canvas_sz, ImVec2 origin)
{
	for each (glm::vec2 p in glm_points)
	{
		points->push_back(ImVec2((p.x) * canvas_sz.x + origin.x, (1 - p.y) * canvas_sz.y + origin.y));
	}
}

void VolumeGUI::safeVector(ImVector<ImVec2>* points, std::vector<glm::vec2>* glm_points, ImVec2 canvas_sz, ImVec2 origin)
{
	std::vector<glm::vec2> points_to_glm;
	for each (ImVec2 p in *points)
	{
		points_to_glm.push_back(glm::vec2((p.x - origin.x) / canvas_sz.x, 1 - (p.y - origin.y) / canvas_sz.y));
	}
	*glm_points = points_to_glm;
}

int VolumeGUI::space_for_new_point(ImVector<ImVec2>* points, ImVec2 mouse_pos, float space_between_points)
{
	int i = 0;
	for each (ImVec2 p in *points){
		if (mouse_pos.x < p.x + space_between_points) {
			if (mouse_pos.x < p.x - space_between_points) {
				return i;
			}
			return -1;
		}
		i++;
	}
	return -1;
}

int VolumeGUI::getPointClicked(ImVec2 mouse_pos, ImVector<ImVec2>* points, float detecting_radius)
{
	int i = 0;
	for each (ImVec2 cur in *points)
	{
		if (distance(cur, mouse_pos) < detecting_radius) {
			return i;
		}
		i++;
	}
	return -1;
}
