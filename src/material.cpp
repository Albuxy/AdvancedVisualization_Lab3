#include "material.h"
#include "texture.h"
#include "application.h"

StandardMaterial::StandardMaterial()
{
	color = vec4(1.f, 1.f, 1.f, 1.f);
	shader = Shader::Get("data/shaders/basic.vs", "data/shaders/flat.fs");
}

StandardMaterial::~StandardMaterial()
{

}

void StandardMaterial::setUniforms(Camera* camera, Matrix44 model)
{
	//upload node uniforms
	shader->setUniform("u_viewprojection", camera->viewprojection_matrix);
	shader->setUniform("u_camera_position", camera->eye);
	shader->setUniform("u_model", model);
	shader->setUniform("u_time", Application::instance->time);

	shader->setUniform("u_color", color);

	if (texture)
		shader->setUniform("u_texture", texture);

	Matrix44 local_model = model;
	local_model.inverse();
	shader->setUniform("u_camera_position_local", local_model*camera->eye);

	//uniforms jittering
	shader->setUniform("u_noiseTexture", noisetexture);
	shader->setUniform("u_jittering_bool", active_jittering);
	shader->setUniform("u_stepsize", stepsize);

	//uniforms clipping
	shader->setUniform("u_plane", plane);
	shader->setUniform("u_clipping_bool", active_clipping);

	//uniforms TF
	shader->setUniform("u_TF_bool", active_TF);
	shader->setUniform("u_density_value_1", density_value_1);
	shader->setUniform("u_density_value_2", density_value_2);
}

void StandardMaterial::render(Mesh* mesh, Matrix44 model, Camera* camera)
{
	//set flags
	glEnable(GL_DEPTH_TEST);
	glDisable(GL_CULL_FACE);

	if (mesh && shader)
	{
		//enable shader
		shader->enable();

		//upload uniforms
		setUniforms(camera, model);

		//do the draw call
		mesh->render(GL_TRIANGLES);

		//disable shader
		shader->disable();
	}
}

void StandardMaterial::renderInMenu()
{
	ImGui::ColorEdit3("Color", (float*)&color); // Edit 3 floats representing a color
	ImGui::Checkbox("Jittering", &active_jittering);
	ImGui::Checkbox("Clipping", &active_clipping);
	ImGui::Checkbox("TransferFunction", &active_TF);

	ImGui::SliderFloat("Step Size", (float*)&stepsize, float(0.0001), float(0.9999));
	ImGui::SliderFloat("plane a", (float*)&plane.x, float(-1.0), float(1.0));
	ImGui::SliderFloat("plane b", (float*)&plane.y, float(-1.0), float(1.0));
	ImGui::SliderFloat("plane c", (float*)&plane.z, float(-1.0), float(1.0));
	ImGui::SliderFloat("plane d", (float*)&plane.w, float(-1.0), float(1.0));
	ImGui::SliderFloat("Density value 1", (float*)&density_value_1, float(0.0), float(1.0));
	ImGui::SliderFloat("Density value 2", (float*)&density_value_2, float(0.0), float(1.0));
}

WireframeMaterial::WireframeMaterial()
{
	color = vec4(1.f, 1.f, 1.f, 1.f);
	shader = Shader::Get("data/shaders/basic.vs", "data/shaders/flat.fs");
}

WireframeMaterial::~WireframeMaterial()
{

}

void WireframeMaterial::render(Mesh* mesh, Matrix44 model, Camera * camera)
{
	if (shader && mesh)
	{
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

		//enable shader
		shader->enable();

		//upload material specific uniforms
		setUniforms(camera, model);

		//do the draw call
		mesh->render(GL_TRIANGLES);

		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	}
}
