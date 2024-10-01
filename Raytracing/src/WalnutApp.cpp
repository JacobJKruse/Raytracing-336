#pragma once
#include "Walnut/Application.h"
#include "Walnut/EntryPoint.h"

#include "Walnut/Image.h"
#include "Walnut/Timer.h"
#include "Renderer.h"
#include "Camera.h"
#include <glm/gtc/type_ptr.hpp>
#include "hittable.h"
#include "hittable_list.h"
#include "sphere.h"
#include "rtweekend.h"
using namespace Walnut;

class ExampleLayer : public Walnut::Layer
{
public:
	ExampleLayer() : camera(45.0f, 0.1f, 100.0f) {}
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
		hittable_list world;

		world.add(make_shared<sphere>(point3(0, 0, -1), 0.5));
		world.add(make_shared<sphere>(point3(0, -100.5, -1), 100));

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