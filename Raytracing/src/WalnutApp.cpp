#pragma once
#include "Walnut/Application.h"
#include "Walnut/EntryPoint.h"

#include "Walnut/Image.h"
#include "Walnut/Timer.h"
#include "Renderer.h"
#include "Camera.h"
#include <glm/gtc/type_ptr.hpp>
#include "bvh.h"
#include "rtweekend.h"
#include "hittable.h"
#include "hittable_list.h"
#include "material.h"
#include "sphere.h"
#include "Scene.h"




using namespace Walnut;

class ExampleLayer : public Walnut::Layer
{
public:
	ExampleLayer() : camera(45.0f, 0.1f, 100.0f) {
		m_Scene = create_scene();
	}
	bool click = false;
	virtual void OnUpdate(float ts) override
	{
		camera.OnUpdate(ts);
	}

	virtual void OnUIRender() override
	{
		ImGui::Begin("Settings");
		ImGui::Text("Last Render Time %.3fms", RenderTime);
		if (ImGui::Button("Render")) {
			Render();
			SaveRenderImage();
			click = true;

		}
		ImGui::End();

		ImGui::Begin("Object Properties");
		for (size_t i = 0; i < m_Scene.object_properties.size(); ++i) {
			auto& prop = m_Scene.object_properties[i];
			ImGui::PushID(i);

			ImGui::Text("Object %zu", i);
			glm::vec3 position = to_glm(prop.position);
			if (ImGui::DragFloat3("Position", glm::value_ptr(position), 0.1f)) {
				prop.position = from_glm(position);
			}
			ImGui::PopID();
		}
		ImGui::End();

		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
		ImGui::Begin("Viewport");

		vpWidth = ImGui::GetContentRegionAvail().x;
		vpHeight = ImGui::GetContentRegionAvail().y;

		auto img = renderer.GetFinalImage();
		if (img) {
			ImGui::Image(img->GetDescriptorSet(), { (float)img->GetWidth(),(float)img->GetHeight() }, ImVec2(0, 1), ImVec2(1, 0));
		}

		ImGui::End();
		ImGui::PopStyleVar();
		if(click)
			Render();

	}


	void Render()
	{
		Timer timer;
		hittable_list world = m_Scene.create_hittable_list();


		renderer.OnResize(vpWidth, vpHeight);
		camera.OnResize(vpWidth, vpHeight);
		renderer.Render(camera, world);


		RenderTime = timer.ElapsedMillis();
	}

	void SaveRenderImage() {

	}
private:
	Renderer renderer;
	uint32_t vpWidth = 0, vpHeight = 0;
	Camera camera;
	float RenderTime = 0.0f;
	Scene m_Scene;

};

Walnut::Application* Walnut::CreateApplication(int argc, char** argv)
{
	Walnut::ApplicationSpecification spec;
	spec.Name = "Ray Tracing";

	Walnut::Application* app = new Walnut::Application(spec);
	app->PushLayer<ExampleLayer>();
	app->SetMenubarCallback([app]()
		{
			if (ImGui::BeginMenu("File"))
			{
				if (ImGui::MenuItem("Exit"))
				{
					app->Close();
				}
				ImGui::EndMenu();
			}
		});
	return app;
}