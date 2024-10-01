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

		auto material_ground = make_shared<lambertian>(color(0.8, 0.8, 0.0));
		auto material_center = make_shared<lambertian>(color(0.1, 0.2, 0.5));
		auto material_left = make_shared<dielectric>(1.50);
		auto material_bubble = make_shared<dielectric>(1.00 / 1.50);
		auto material_right = make_shared<metal>(color(0.8, 0.6, 0.2), 0.0);

		world.add(make_shared<sphere>(point3(0.0, -100.5, -1.0), 100.0, material_ground));
		world.add(make_shared<sphere>(point3(0.0, 0.0, -1.2), 0.5, material_center));
		world.add(make_shared<sphere>(point3(-1.0, 0.0, -1.0), 0.5, material_left));
		world.add(make_shared<sphere>(point3(-1.0, 0.0, -1.0), 0.4, material_bubble));
		world.add(make_shared<sphere>(point3(1.0, 0.0, -1.0), 0.5, material_right));

		world = hittable_list(make_shared<bvh_node>(world));

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