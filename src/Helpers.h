#pragma once

#include "ofGLBaseTypes.h"
#include "ofParameter.h"
#include "ofRectangle.h"
#include "ofTexture.h"
#include "ofGLBaseTypes.h"
#include "imgui.h"

static const int kImGuiMargin = 10;


namespace ImGui {
    IMGUI_API bool Checkbox(ofParameter<bool>& v);
    IMGUI_API bool Checkbox(const char* label, ofParameter<bool>* v);
    IMGUI_API bool CheckboxFlags(ofParameter<unsigned int>& flags, unsigned int flags_value);
    IMGUI_API bool CheckboxFlags(const char* label, ofParameter<unsigned int>* flags, unsigned int flags_value);
    IMGUI_API bool RadioButton(ofParameter<int>& v, int v_button);
    IMGUI_API bool RadioButton(const char* label, ofParameter<int>* v, int v_button);
    IMGUI_API bool InputText(const char* label, std::string* str, ImGuiInputTextFlags flags = ImGuiInputTextFlags_CallbackResize, ImGuiInputTextCallback callback = nullptr, void* user_data = nullptr);
    IMGUI_API bool InputTextMultiline(const char* label, std::string* str, ImVec2 const& size = ImVec2(0, 0), ImGuiInputTextFlags flags = ImGuiInputTextFlags_CallbackResize, ImGuiInputTextCallback callback = nullptr, void* user_data = nullptr);
    IMGUI_API bool InputText(ofParameter<std::string>& str, ImGuiInputTextFlags flags = ImGuiInputTextFlags_CallbackResize, ImGuiInputTextCallback callback = nullptr, void* user_data = nullptr);
    IMGUI_API bool InputTextMultiline(ofParameter<std::string>& str, ImVec2 const& size = ImVec2(0, 0), ImGuiInputTextFlags flags = ImGuiInputTextFlags_CallbackResize, ImGuiInputTextCallback callback = nullptr, void* user_data = nullptr);
    IMGUI_API bool InputText(const char* label, ofParameter<std::string>* str, ImGuiInputTextFlags flags = ImGuiInputTextFlags_CallbackResize, ImGuiInputTextCallback callback = nullptr, void* user_data = nullptr);
    IMGUI_API bool InputTextMultiline(const char* label, ofParameter<std::string>* str, ImVec2 const& size = ImVec2(0, 0), ImGuiInputTextFlags flags = ImGuiInputTextFlags_CallbackResize, ImGuiInputTextCallback callback = nullptr, void* user_data = nullptr);
    IMGUI_API bool DragFloat(ofParameter<float>& v, float v_speed = 1.0f, float v_min = 0.0f, float v_max = 0.0f, const char* format = "%.3f", float power = 1.0f);
    IMGUI_API bool DragFloat(const char* label, ofParameter<float>* v, float v_speed = 1.0f, float v_min = 0.0f, float v_max = 0.0f, const char* format = "%.3f", float power = 1.0f);
    IMGUI_API bool DragFloat2(const char* label, ofVec2f* v, float v_speed = 1.0f, float v_min = 0.0f, float v_max = 0.0f, const char* format = "%.3f", float power = 1.0f);
    IMGUI_API bool DragFloat2(ofParameter<ofVec2f>& v, float v_speed = 1.0f, float v_min = 0.0f, float v_max = 0.0f, const char* format = "%.3f", float power = 1.0f);
    IMGUI_API bool DragFloat2(const char* label, ofParameter<ofVec2f>* v, float v_speed = 1.0f, float v_min = 0.0f, float v_max = 0.0f, const char* format = "%.3f", float power = 1.0f);
    IMGUI_API bool DragFloat3(const char* label, ofVec3f* v, float v_speed = 1.0f, float v_min = 0.0f, float v_max = 0.0f, const char* format = "%.3f", float power = 1.0f);
    IMGUI_API bool DragFloat3(ofParameter<ofVec3f>& v, float v_speed = 1.0f, float v_min = 0.0f, float v_max = 0.0f, const char* format = "%.3f", float power = 1.0f);
    IMGUI_API bool DragFloat3(const char* label, ofParameter<ofVec3f>* v, float v_speed = 1.0f, float v_min = 0.0f, float v_max = 0.0f, const char* format = "%.3f", float power = 1.0f);
    IMGUI_API bool DragFloat4(const char* label, ofVec4f* v, float v_speed = 1.0f, float v_min = 0.0f, float v_max = 0.0f, const char* format = "%.3f", float power = 1.0f);
    IMGUI_API bool DragFloat4(ofParameter<ofVec4f>& v, float v_speed = 1.0f, float v_min = 0.0f, float v_max = 0.0f, const char* format = "%.3f", float power = 1.0f);
    IMGUI_API bool DragFloat4(const char* label, ofParameter<ofVec4f>* v, float v_speed = 1.0f, float v_min = 0.0f, float v_max = 0.0f, const char* format = "%.3f", float power = 1.0f);
    IMGUI_API bool DragFloatRange2(const char* label, ofParameter<float>* v_current_min, ofParameter<float>* v_current_max, float v_speed = 1.0f, float v_min = 0.0f, float v_max = 0.0f, const char* format = "%.3f", const char* format_max = NULL, float power = 1.0f);
    IMGUI_API bool DragInt(ofParameter<int>& v, float v_speed = 1.0f, int v_min = 0, int v_max = 0, const char* format = "%d");                                       // If v_min >= v_max we have no bound
    IMGUI_API bool DragInt(const char* label, ofParameter<int>* v, float v_speed = 1.0f, int v_min = 0, int v_max = 0, const char* format = "%d");                    // If v_min >= v_max we have no bound
    IMGUI_API bool DragIntRange2(const char* label, ofParameter<int>* v_current_min, ofParameter<int>* v_current_max, float v_speed = 1.0f, int v_min = 0, int v_max = 0, const char* format = "%d", const char* format_max = NULL);
    IMGUI_API bool SliderFloat(ofParameter<float>& v, float v_min, float v_max, const char* format = "%.3f", float power = 1.0f);     // adjust format to decorate the value with a prefix or a suffix for in-slider labels or unit display. Use power!=1.0 for power curve sliders
    IMGUI_API bool SliderFloat(const char* label, ofParameter<float>* v, float v_min, float v_max, const char* format = "%.3f", float power = 1.0f);     // adjust format to decorate the value with a prefix or a suffix for in-slider labels or unit display. Use power!=1.0 for power curve sliders
    IMGUI_API bool SliderFloat2(const char* label, ofVec2f* v, float v_min, float v_max, const char* format = "%.3f", float power = 1.0f);
    IMGUI_API bool SliderFloat2(ofParameter<ofVec2f>& v, float v_min, float v_max, const char* format = "%.3f", float power = 1.0f);
    IMGUI_API bool SliderFloat2(const char* label, ofParameter<ofVec2f>* v, float v_min, float v_max, const char* format = "%.3f", float power = 1.0f);
    IMGUI_API bool SliderFloat3(const char* label, ofVec3f* v, float v_min, float v_max, const char* format = "%.3f", float power = 1.0f);
    IMGUI_API bool SliderFloat3(ofParameter<ofVec3f>& v, float v_min, float v_max, const char* format = "%.3f", float power = 1.0f);
    IMGUI_API bool SliderFloat3(const char* label, ofParameter<ofVec3f>* v, float v_min, float v_max, const char* format = "%.3f", float power = 1.0f);
    IMGUI_API bool SliderFloat4(const char* label, ofVec4f* v, float v_min, float v_max, const char* format = "%.3f", float power = 1.0f);
    IMGUI_API bool SliderFloat4(ofParameter<ofVec4f>& v, float v_min, float v_max, const char* format = "%.3f", float power = 1.0f);
    IMGUI_API bool SliderFloat4(const char* label, ofParameter<ofVec4f>* v, float v_min, float v_max, const char* format = "%.3f", float power = 1.0f);
    IMGUI_API bool SliderAngle(ofParameter<float>& v_rad, float v_degrees_min = -360.0f, float v_degrees_max = +360.0f);
    IMGUI_API bool SliderAngle(const char* label, ofParameter<float>* v_rad, float v_degrees_min = -360.0f, float v_degrees_max = +360.0f);
    IMGUI_API bool SliderInt(ofParameter<int>& v, int v_min, int v_max, const char* format = "%d");
    IMGUI_API bool SliderInt(const char* label, ofParameter<int>* v, int v_min, int v_max, const char* format = "%d");
    IMGUI_API bool VSliderFloat(const ImVec2& size, ofParameter<float>& v, float v_min, float v_max, const char* format = "%.3f", float power = 1.0f);
    IMGUI_API bool VSliderFloat(const char* label, const ImVec2& size, ofParameter<float>* v, float v_min, float v_max, const char* format = "%.3f", float power = 1.0f);
    IMGUI_API bool VSliderInt(const ImVec2& size, ofParameter<int>& v, int v_min, int v_max, const char* format = "%d");
    IMGUI_API bool VSliderInt(const char* label, const ImVec2& size, ofParameter<int>* v, int v_min, int v_max, const char* format = "%d");
    IMGUI_API bool ColorEdit3(const char* label, ofColor *color, ImGuiColorEditFlags flags = 0);
    IMGUI_API bool ColorEdit3(const char* label, ofFloatColor *color, ImGuiColorEditFlags flags = 0);
    IMGUI_API bool ColorEdit4(const char* label, ofColor *color, ImGuiColorEditFlags flags = 0);
    IMGUI_API bool ColorEdit4(const char* label, ofFloatColor *color, ImGuiColorEditFlags flags = 0);
    IMGUI_API bool ColorPicker3(const char* label, ofColor *color, ImGuiColorEditFlags flags = 0);
    IMGUI_API bool ColorPicker3(const char* label, ofFloatColor *color, ImGuiColorEditFlags flags = 0);
    IMGUI_API bool ColorPicker4(const char* label, ofColor *color, ImGuiColorEditFlags flags = 0, const ofColor* ref_col = NULL);
    IMGUI_API bool ColorPicker4(const char* label, ofFloatColor *color, ImGuiColorEditFlags flags = 0, const ofFloatColor* ref_col = NULL);
    IMGUI_API bool ColorButton(const char* desc_id, ofColor const& col, ImGuiColorEditFlags flags = 0, ImVec2 size = ImVec2(0,0));  // display a colored square/button, hover for details, return true when pressed.
    IMGUI_API bool ColorButton(const char* desc_id, ofFloatColor const& col, ImGuiColorEditFlags flags = 0, ImVec2 size = ImVec2(0,0));  // display a colored square/button, hover for details, return true when pressed.
    IMGUI_API void Text(std::string const& text);
    IMGUI_API void TextColored(ImVec4 const& col, std::string const& text);
    IMGUI_API void TextDisabled(std::string const& text);
    IMGUI_API void TextWrapped(std::string const& text);
    IMGUI_API void LabelText(const char* label, std::string const& text);
    IMGUI_API void BulletText(std::string const& text);
    IMGUI_API void Image(const ofBaseHasTexture& image, const ImVec2& size, const ImVec2& uv0 = ImVec2(0,0), const ImVec2& uv1 = ImVec2(1,1), const ImVec4& tint_col = ImVec4(1,1,1,1), const ImVec4& border_col = ImVec4(0,0,0,0));
    IMGUI_API bool ImageButton(const ofBaseHasTexture& image, const ImVec2& size, const ImVec2& uv0 = ImVec2(0,0),  const ImVec2& uv1 = ImVec2(1,1), int frame_padding = -1, const ImVec4& bg_col = ImVec4(0,0,0,0), const ImVec4& tint_col = ImVec4(1,1,1,1));    // <0 frame_padding uses default frame padding settings. 0 for no padding
    IMGUI_API void Image(const ofTexture& texture, const ImVec2& size, const ImVec2& uv0 = ImVec2(0,0), const ImVec2& uv1 = ImVec2(1,1), const ImVec4& tint_col = ImVec4(1,1,1,1), const ImVec4& border_col = ImVec4(0,0,0,0));
    IMGUI_API bool ImageButton(const ofTexture& texture, const ImVec2& size, const ImVec2& uv0 = ImVec2(0,0),  const ImVec2& uv1 = ImVec2(1,1), int frame_padding = -1, const ImVec4& bg_col = ImVec4(0,0,0,0), const ImVec4& tint_col = ImVec4(1,1,1,1));    // <0 frame_padding uses default frame padding settings. 0 for no padding
}

namespace ofxImGui
{
    
    bool VectorCombo(const char* label, int* currIndex, std::vector<std::string>& values);
    bool VectorListBox(const char* label, int* currIndex, std::vector<std::string>& values);

    
	struct WindowOpen
	{
		std::stack<std::vector<std::string>> usedNames;
		std::shared_ptr<ofParameter<bool>> parameter;
		bool value;
	};

	struct Settings
	{
		Settings();

		ofVec2f windowPos;
		ofVec2f windowSize;
		bool lockPosition;
		
		bool mouseOverGui;
		bool windowBlock;
		int treeLevel;

		ofRectangle totalBounds;
		ofRectangle screenBounds;
	};

	static WindowOpen windowOpen;

	bool IsMouseOverGui();

	const char * GetUniqueName(ofAbstractParameter& parameter);
	const char * GetUniqueName(const std::string& candidate);

	void SetNextWindow(Settings& settings);
	bool BeginWindow(ofParameter<bool>& parameter, Settings& settings, bool collapse = true);
	bool BeginWindow(const std::string& name, Settings& settings, bool collapse = true, bool * open = nullptr);
	bool BeginWindow(const std::string& name, Settings& settings, ImGuiWindowFlags flags, bool * open = nullptr);
	void EndWindow(Settings& settings);

	bool BeginTree(ofAbstractParameter& parameter, Settings& settings);
	bool BeginTree(const std::string& name, Settings& settings);
	void EndTree(Settings& settings);

	void AddGroup(ofParameterGroup& group, Settings& settings);

#if OF_VERSION_MINOR >= 10
	bool AddParameter(ofParameter<glm::ivec2>& parameter);
	bool AddParameter(ofParameter<glm::ivec3>& parameter);
	bool AddParameter(ofParameter<glm::ivec4>& parameter);

	bool AddParameter(ofParameter<glm::vec2>& parameter);
	bool AddParameter(ofParameter<glm::vec3>& parameter);
	bool AddParameter(ofParameter<glm::vec4>& parameter);
#endif

	bool AddParameter(ofParameter<ofVec2f>& parameter);
	bool AddParameter(ofParameter<ofVec3f>& parameter);
	bool AddParameter(ofParameter<ofVec4f>& parameter);

	bool AddParameter(ofParameter<ofFloatColor>& parameter, bool alpha = true);

	bool AddParameter(ofParameter<std::string>& parameter, size_t maxChars = 255, bool multiline = false);

	bool AddParameter(ofParameter<void>& parameter);

	template<typename ParameterType>
	bool AddParameter(ofParameter<ParameterType>& parameter);

	bool AddRadio(ofParameter<int>& parameter, std::vector<std::string> labels, int columns = 1);
	bool AddCombo(ofParameter<int>& parameter, std::vector<std::string> labels);
	bool AddStepper(ofParameter<int>& parameter, int step = 1, int stepFast = 100);

	bool AddRange(const std::string& name, ofParameter<int>& parameterMin, ofParameter<int>& parameterMax, int speed = 1);
	bool AddRange(const std::string& name, ofParameter<float>& parameterMin, ofParameter<float>& parameterMax, float speed = 0.01f);
#if OF_VERSION_MINOR >= 10
	bool AddRange(const std::string& name, ofParameter<glm::vec2>& parameterMin, ofParameter<glm::vec2>& parameterMax, float speed = 0.01f);
	bool AddRange(const std::string& name, ofParameter<glm::vec3>& parameterMin, ofParameter<glm::vec3>& parameterMax, float speed = 0.01f);
	bool AddRange(const std::string& name, ofParameter<glm::vec4>& parameterMin, ofParameter<glm::vec4>& parameterMax, float speed = 0.01f);
#endif

#if OF_VERSION_MINOR >= 10
	bool AddValues(const std::string& name, std::vector<glm::ivec2>& values, int minValue, int maxValue);
	bool AddValues(const std::string& name, std::vector<glm::ivec3>& values, int minValue, int maxValue);
	bool AddValues(const std::string& name, std::vector<glm::ivec4>& values, int minValue, int maxValue);

	bool AddValues(const std::string& name, std::vector<glm::vec2>& values, float minValue, float maxValue);
	bool AddValues(const std::string& name, std::vector<glm::vec3>& values, float minValue, float maxValue);
	bool AddValues(const std::string& name, std::vector<glm::vec4>& values, float minValue, float maxValue);
#endif

	bool AddValues(const std::string& name, std::vector<ofVec2f>& values, float minValue, float maxValue);
	bool AddValues(const std::string& name, std::vector<ofVec3f>& values, float minValue, float maxValue);
	bool AddValues(const std::string& name, std::vector<ofVec4f>& values, float minValue, float maxValue);

	template<typename DataType>
	bool AddValues(const std::string& name, std::vector<DataType>& values, DataType minValue, DataType maxValue);

	void AddImage(ofBaseHasTexture& hasTexture, const ofVec2f& size);
	void AddImage(ofTexture& texture, const ofVec2f& size);
}

static ImTextureID GetImTextureID(ofTexture& texture)
{
    return (ImTextureID)(uintptr_t)texture.texData.textureID;
}

static ImTextureID GetImTextureID(ofBaseHasTexture& hasTexture)
{
    
    return GetImTextureID(hasTexture.getTexture());
}

static ImTextureID GetImTextureID(GLuint glID)
{
    
    return (ImTextureID)(uintptr_t)glID;

}

//--------------------------------------------------------------
template<typename ParameterType>
bool ofxImGui::AddParameter(ofParameter<ParameterType>& parameter)
{
	auto tmpRef = parameter.get();
	const auto& info = typeid(ParameterType);
	if (info == typeid(float))
	{
		if (ImGui::SliderFloat(GetUniqueName(parameter), (float *)&tmpRef, parameter.getMin(), parameter.getMax()))
		{
			parameter.set(tmpRef);
			return true;
		}
		return false;
	}
	if (info == typeid(int))
	{
		if (ImGui::SliderInt(GetUniqueName(parameter), (int *)&tmpRef, parameter.getMin(), parameter.getMax()))
		{
			parameter.set(tmpRef);
			return true;
		}
		return false;
	}
	if (info == typeid(bool))
	{
		if (ImGui::Checkbox(GetUniqueName(parameter), (bool *)&tmpRef))
		{
			parameter.set(tmpRef);
			return true;
		}
		return false;
	}

	ofLogWarning(__FUNCTION__) << "Could not create GUI element for type " << info.name();
	return false;
}

//--------------------------------------------------------------
template<typename DataType>
bool ofxImGui::AddValues(const std::string& name, std::vector<DataType>& values, DataType minValue, DataType maxValue)
{
	auto result = false;
	const auto& info = typeid(DataType);
	for (int i = 0; i < values.size(); ++i)
	{
		const auto iname = name + " " + ofToString(i);
		if (info == typeid(float))
		{
			result |= ImGui::SliderFloat(GetUniqueName(iname), *values[i], minValue, maxValue);
		}
		else if (info == typeid(int))
		{
			result |= ImGui::SliderInt(GetUniqueName(iname), *values[i], minValue, maxValue);
		}
		else if (info == typeid(bool))
		{
			result |= ImGui::Checkbox(GetUniqueName(iname), *values[i]);
		}
		else
		{
			ofLogWarning("Gui::AddValues") << "Could not create GUI element for type " << info.name();
			return false;
		}
	}
	return result;
}
