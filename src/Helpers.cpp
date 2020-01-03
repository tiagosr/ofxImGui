#include "Helpers.h"

//--------------------------------------------------------------
ofxImGui::Settings::Settings()
	: windowPos(kImGuiMargin, kImGuiMargin)
	, windowSize(ofVec2f::zero())
	, lockPosition(false)
	, windowBlock(false)
	, mouseOverGui(false)
    , treeLevel(0)
{}

bool ofxImGui::IsMouseOverGui()
{
	return ImGui::IsWindowHovered(ImGuiHoveredFlags_AnyWindow);
}

//--------------------------------------------------------------
const char * ofxImGui::GetUniqueName(ofAbstractParameter& parameter)
{
	return GetUniqueName(parameter.getName());
}

//--------------------------------------------------------------
const char * ofxImGui::GetUniqueName(const std::string& candidate)
{
	std::string result = candidate;
	while (std::find(windowOpen.usedNames.top().begin(), windowOpen.usedNames.top().end(), result) != windowOpen.usedNames.top().end())
	{
		result += " ";
	}
	windowOpen.usedNames.top().push_back(result);
	return windowOpen.usedNames.top().back().c_str();
}

//--------------------------------------------------------------
void ofxImGui::SetNextWindow(Settings& settings)
{
	settings.windowSize.x = 0;
	settings.windowPos.y += settings.windowSize.y + kImGuiMargin;
}

//--------------------------------------------------------------
bool ofxImGui::BeginWindow(ofParameter<bool>& parameter, Settings& settings, bool collapse)
{
	if (settings.windowBlock)
	{
		ofLogWarning(__FUNCTION__) << "Already inside a window block!";
		return false;
	}

	// Reference this ofParameter until EndWindow().
	windowOpen.parameter = std::dynamic_pointer_cast<ofParameter<bool>>(parameter.newReference());
	windowOpen.value = parameter.get();

	auto result = ofxImGui::BeginWindow(parameter.getName(), settings, collapse, &windowOpen.value);
	parameter = windowOpen.value;
	return result;
}

//--------------------------------------------------------------
bool ofxImGui::BeginWindow(const std::string& name, Settings& settings, bool collapse, bool * open)
{
	if (settings.windowBlock)
	{
		ofLogWarning(__FUNCTION__) << "Already inside a window block!";
		return false;
	}

	settings.windowBlock = true;

	// Push a new list of names onto the stack.
	windowOpen.usedNames.push(std::vector<std::string>());

	ImGui::SetNextWindowPos(settings.windowPos, settings.lockPosition? ImGuiCond_Always : ImGuiCond_Appearing);
	ImGui::SetNextWindowSize(settings.windowSize, ImGuiCond_Appearing);
	ImGui::SetNextWindowCollapsed(collapse, ImGuiCond_Appearing);
	return ImGui::Begin(name.c_str(), open, ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_AlwaysAutoResize | (collapse ? 0 : ImGuiWindowFlags_NoCollapse));
}

//--------------------------------------------------------------
bool ofxImGui::BeginWindow(const std::string& name, Settings& settings, ImGuiWindowFlags flags, bool * open)
{
	if (settings.windowBlock)
	{
		ofLogWarning(__FUNCTION__) << "Already inside a window block!";
		return false;
	}

	settings.windowBlock = true;

	// Push a new list of names onto the stack.
	windowOpen.usedNames.push(std::vector<std::string>());

	ImGui::SetNextWindowPos(settings.windowPos, settings.lockPosition? ImGuiCond_Always : ImGuiCond_Appearing);
	ImGui::SetNextWindowSize(settings.windowSize, ImGuiCond_Appearing);
	ImGui::SetNextWindowCollapsed(!(flags & ImGuiWindowFlags_NoCollapse), ImGuiCond_Appearing);
	return ImGui::Begin(name.c_str(), open, flags);
}

//--------------------------------------------------------------
void ofxImGui::EndWindow(Settings& settings)
{
	if (!settings.windowBlock)
	{
		ofLogWarning(__FUNCTION__) << "Not inside a window block!";
		return;
	}

	settings.windowBlock = false;

	settings.windowPos = ImGui::GetWindowPos();
	settings.windowSize = ImGui::GetWindowSize();
	settings.mouseOverGui |= ImGui::IsWindowHovered(ImGuiHoveredFlags_AnyWindow);
	ImGui::End();

	// Unlink the referenced ofParameter.
	windowOpen.parameter.reset();

	// Clear the list of names from the stack.
	windowOpen.usedNames.pop();

	// Include this window's bounds in the total bounds.
	const auto windowBounds = ofRectangle(settings.windowPos, settings.windowSize.x, settings.windowSize.y);
	if (settings.totalBounds.isZero())
	{
		settings.totalBounds = windowBounds;
	}
	else
	{
		settings.totalBounds.growToInclude(windowBounds);
	}
}

//--------------------------------------------------------------
bool ofxImGui::BeginTree(ofAbstractParameter& parameter, Settings& settings)
{
	return ofxImGui::BeginTree(parameter.getName(), settings);
}

//--------------------------------------------------------------
bool ofxImGui::BeginTree(const std::string& name, Settings& settings)
{
	bool result;
	ImGui::SetNextTreeNodeOpen(true, ImGuiSetCond_Appearing);
	if (settings.treeLevel == 0)
	{
		result = ImGui::TreeNodeEx(GetUniqueName(name), ImGuiTreeNodeFlags_Framed | ImGuiTreeNodeFlags_NoAutoOpenOnLog);
	}
	else
	{
		result = ImGui::TreeNode(GetUniqueName(name));
	}
	if (result)
	{
		settings.treeLevel += 1;

		// Push a new list of names onto the stack.
		windowOpen.usedNames.push(std::vector<std::string>());
	}
	return result;
}

//--------------------------------------------------------------
void ofxImGui::EndTree(Settings& settings)
{
	ImGui::TreePop();
	
	settings.treeLevel = std::max(0, settings.treeLevel - 1);

	// Clear the list of names from the stack.
	windowOpen.usedNames.pop();
}

//--------------------------------------------------------------
void ofxImGui::AddGroup(ofParameterGroup& group, Settings& settings)
{
	bool prevWindowBlock = settings.windowBlock;
	if (settings.windowBlock)
	{
		if (!ofxImGui::BeginTree(group, settings))
		{
			return;
		}
	}
	else
	{
		if (!ofxImGui::BeginWindow(group.getName().c_str(), settings))
		{
			ofxImGui::EndWindow(settings);
			return;
		}
	}

	for (auto parameter : group)
	{
		// Group.
		auto parameterGroup = std::dynamic_pointer_cast<ofParameterGroup>(parameter);
		if (parameterGroup)
		{
			// Recurse through contents.
			ofxImGui::AddGroup(*parameterGroup, settings);
			continue;
		}

		// Parameter, try everything we know how to handle.
#if OF_VERSION_MINOR >= 10
		auto parameterVec2f = std::dynamic_pointer_cast<ofParameter<glm::vec2>>(parameter);
		if (parameterVec2f)
		{
			ofxImGui::AddParameter(*parameterVec2f);
			continue;
		}
		auto parameterVec3f = std::dynamic_pointer_cast<ofParameter<glm::vec3>>(parameter);
		if (parameterVec3f)
		{
			ofxImGui::AddParameter(*parameterVec3f);
			continue;
		}
		auto parameterVec4f = std::dynamic_pointer_cast<ofParameter<glm::vec4>>(parameter);
		if (parameterVec4f)
		{
			ofxImGui::AddParameter(*parameterVec4f);
			continue;
		}
#endif
		auto parameterOfVec2f = std::dynamic_pointer_cast<ofParameter<ofVec2f>>(parameter);
		if (parameterOfVec2f)
		{
			ofxImGui::AddParameter(*parameterOfVec2f);
			continue;
		}
		auto parameterOfVec3f = std::dynamic_pointer_cast<ofParameter<ofVec3f>>(parameter);
		if (parameterOfVec3f)
		{
			ofxImGui::AddParameter(*parameterOfVec3f);
			continue;
		}
		auto parameterOfVec4f = std::dynamic_pointer_cast<ofParameter<ofVec4f>>(parameter);
		if (parameterOfVec4f)
		{
			ofxImGui::AddParameter(*parameterOfVec4f);
			continue;
		}
		auto parameterFloatColor = std::dynamic_pointer_cast<ofParameter<ofFloatColor>>(parameter);
		if (parameterFloatColor)
		{
			ofxImGui::AddParameter(*parameterFloatColor);
			continue;
		}
		auto parameterFloat = std::dynamic_pointer_cast<ofParameter<float>>(parameter);
		if (parameterFloat)
		{
			ofxImGui::AddParameter(*parameterFloat);
			continue;
		}
		auto parameterInt = std::dynamic_pointer_cast<ofParameter<int>>(parameter);
		if (parameterInt)
		{
			ofxImGui::AddParameter(*parameterInt);
			continue;
		}
		auto parameterBool = std::dynamic_pointer_cast<ofParameter<bool>>(parameter);
		if (parameterBool)
		{
			ofxImGui::AddParameter(*parameterBool);
			continue;
		}

		ofLogWarning(__FUNCTION__) << "Could not create GUI element for parameter " << parameter->getName();
	}

	if (settings.windowBlock && !prevWindowBlock)
	{
		// End window if we created it.
		ofxImGui::EndWindow(settings);
	}
	else
	{
		// End tree.
		ofxImGui::EndTree(settings);
	}
}

#if OF_VERSION_MINOR >= 10

//--------------------------------------------------------------
bool ofxImGui::AddParameter(ofParameter<glm::tvec2<int>>& parameter)
{
	auto tmpRef = parameter.get();
	if (ImGui::SliderInt2(GetUniqueName(parameter), glm::value_ptr(tmpRef), parameter.getMin().x, parameter.getMax().x))
	{
		parameter.set(tmpRef);
		return true;
	}
	return false;
}

//--------------------------------------------------------------
bool ofxImGui::AddParameter(ofParameter<glm::tvec3<int>>& parameter)
{
	auto tmpRef = parameter.get();
	if (ImGui::SliderInt3(GetUniqueName(parameter), glm::value_ptr(tmpRef), parameter.getMin().x, parameter.getMax().x))
	{
		parameter.set(tmpRef);
		return true;
	}
	return false;
}

//--------------------------------------------------------------
bool ofxImGui::AddParameter(ofParameter<glm::tvec4<int>>& parameter)
{
	auto tmpRef = parameter.get();
	if (ImGui::SliderInt4(GetUniqueName(parameter), glm::value_ptr(tmpRef), parameter.getMin().x, parameter.getMax().x))
	{
		parameter.set(tmpRef);
		return true;
	}
	return false;
}

//--------------------------------------------------------------
bool ofxImGui::AddParameter(ofParameter<glm::vec2>& parameter)
{
	auto tmpRef = parameter.get();
	if (ImGui::SliderFloat2(GetUniqueName(parameter), glm::value_ptr(tmpRef), parameter.getMin().x, parameter.getMax().x))
	{
		parameter.set(tmpRef);
		return true;
	}
	return false;
}

//--------------------------------------------------------------
bool ofxImGui::AddParameter(ofParameter<glm::vec3>& parameter)
{
	auto tmpRef = parameter.get();
	if (ImGui::SliderFloat3(GetUniqueName(parameter), glm::value_ptr(tmpRef), parameter.getMin().x, parameter.getMax().x))
	{
		parameter.set(tmpRef);
		return true;
	}
	return false;
}

//--------------------------------------------------------------
bool ofxImGui::AddParameter(ofParameter<glm::vec4>& parameter)
{
	auto tmpRef = parameter.get();
	if (ImGui::SliderFloat4(GetUniqueName(parameter), glm::value_ptr(tmpRef), parameter.getMin().x, parameter.getMax().x))
	{
		parameter.set(tmpRef);
		return true;
	}
	return false;
}

#endif

//--------------------------------------------------------------
bool ofxImGui::AddParameter(ofParameter<ofVec2f>& parameter)
{
	auto tmpRef = parameter.get();
	if (ImGui::SliderFloat2(GetUniqueName(parameter), tmpRef.getPtr(), parameter.getMin().x, parameter.getMax().x))
	{
		parameter.set(tmpRef);
		return true;
	}
	return false;
}

//--------------------------------------------------------------
bool ofxImGui::AddParameter(ofParameter<ofVec3f>& parameter)
{
	auto tmpRef = parameter.get();
	if (ImGui::SliderFloat3(GetUniqueName(parameter), tmpRef.getPtr(), parameter.getMin().x, parameter.getMax().x))
	{
		parameter.set(tmpRef);
		return true;
	}
	return false;
}

//--------------------------------------------------------------
bool ofxImGui::AddParameter(ofParameter<ofVec4f>& parameter)
{
	auto tmpRef = parameter.get();
	if (ImGui::SliderFloat4(GetUniqueName(parameter), tmpRef.getPtr(), parameter.getMin().x, parameter.getMax().x))
	{
		parameter.set(tmpRef);
		return true;
	}
	return false;
}

//--------------------------------------------------------------
bool ofxImGui::AddParameter(ofParameter<ofFloatColor>& parameter, bool alpha)
{
	auto tmpRef = parameter.get();
	if (alpha)
	{
		if (ImGui::ColorEdit4(GetUniqueName(parameter), &tmpRef.r))
		{
			parameter.set(tmpRef);
			return true;
		}
	}
	else if (ImGui::ColorEdit3(GetUniqueName(parameter), &tmpRef.r))
	{
		parameter.set(tmpRef);
		return true;
	}
	return false;
}

//--------------------------------------------------------------
bool ofxImGui::AddParameter(ofParameter<std::string>& parameter, size_t maxChars, bool multiline)
{
	auto tmpRef = parameter.get();
	char * cString = new char[maxChars];
	strcpy(cString, tmpRef.c_str());
	auto result = false;
	if (multiline)
	{
		if (ImGui::InputTextMultiline(GetUniqueName(parameter), cString, maxChars))
		{
			parameter.set(cString);
			result = true;
		}
	}
	else if (ImGui::InputText(GetUniqueName(parameter), cString, maxChars))
	{
		parameter.set(cString);
		result = true;
	}
	delete[] cString;
	return result;
}

//--------------------------------------------------------------
bool ofxImGui::AddParameter(ofParameter<void>& parameter)
{
	if (ImGui::Button(GetUniqueName(parameter)))
	{
		parameter.trigger();
		return true;
	}
	return false;
}

//--------------------------------------------------------------
bool ofxImGui::AddRadio(ofParameter<int>& parameter, std::vector<std::string> labels, int columns)
{
	auto uniqueName = GetUniqueName(parameter);
	ImGui::Text("%s", uniqueName);
	auto result = false;
	auto tmpRef = parameter.get();
	ImGui::PushID(uniqueName);
	{
		ImGui::Columns(columns);
		for (int i = 0; i < labels.size(); ++i)
		{
			result |= ImGui::RadioButton(GetUniqueName(labels[i]), &tmpRef, i);
			ImGui::NextColumn();
		}
		ImGui::Columns(1);
	}
	ImGui::PopID();
	if (result)
	{
		parameter.set(tmpRef);
	}
	return result;
}

//--------------------------------------------------------------
bool ofxImGui::AddCombo(ofParameter<int>& parameter, std::vector<std::string> labels)
{
	auto result = false;
	auto tmpRef = parameter.get();
	if (ImGui::BeginCombo(GetUniqueName(parameter), labels.at(parameter.get()).c_str()))
	{
		for (int i = 0; i < labels.size(); ++i)
		{
			bool selected = (i == tmpRef);
			if (ImGui::Selectable(labels[i].c_str(), selected))
			{
				tmpRef = i;
				result = true;
			}
			if (selected)
			{
				ImGui::SetItemDefaultFocus();
			}
		}

		ImGui::EndCombo();
	}
	if (result)
	{
		parameter.set(tmpRef);
	}
	return result;
}

//--------------------------------------------------------------
bool ofxImGui::AddStepper(ofParameter<int>& parameter, int step, int stepFast)
{
	auto tmpRef = parameter.get();
	if (ImGui::InputInt(GetUniqueName(parameter), &tmpRef, step, stepFast))
	{
		parameter.set(tmpRef);
		return true;
	}
	return false;
}

//--------------------------------------------------------------
bool ofxImGui::AddRange(const std::string& name, ofParameter<int>& parameterMin, ofParameter<int>& parameterMax, int speed)
{
	auto tmpRefMin = parameterMin.get();
	auto tmpRefMax = parameterMax.get();
	if (ImGui::DragIntRange2(GetUniqueName(name), &tmpRefMin, &tmpRefMax, speed, parameterMin.getMin(), parameterMax.getMax()))
	{
		parameterMin.set(tmpRefMin);
		parameterMax.set(tmpRefMax);
		return true;
	}
	return false;
}

//--------------------------------------------------------------
bool ofxImGui::AddRange(const std::string& name, ofParameter<float>& parameterMin, ofParameter<float>& parameterMax, float speed)
{
	auto tmpRefMin = parameterMin.get();
	auto tmpRefMax = parameterMax.get();
	if (ImGui::DragFloatRange2(GetUniqueName(name), &tmpRefMin, &tmpRefMax, speed, parameterMin.getMin(), parameterMax.getMax()))
	{
		parameterMin.set(tmpRefMin);
		parameterMax.set(tmpRefMax);
		return true;
	}
	return false;
}

#if OF_VERSION_MINOR >= 10

//--------------------------------------------------------------
bool ofxImGui::AddRange(const std::string& name, ofParameter<glm::vec2>& parameterMin, ofParameter<glm::vec2>& parameterMax, float speed)
{
	auto result = false;
	auto tmpRefMin = parameterMin.get();
	auto tmpRefMax = parameterMax.get();
	if (ImGui::DragFloatRange2(GetUniqueName(name + " X"), &tmpRefMin.x, &tmpRefMax.x, speed, parameterMin.getMin().x, parameterMax.getMax().x))
	{
		result |= true;
	}
	if (ImGui::DragFloatRange2(GetUniqueName(name + " Y"), &tmpRefMin.y, &tmpRefMax.y, speed, parameterMin.getMin().y, parameterMax.getMax().y))
	{
		result |= true;
	}
	if (result)
	{
		parameterMin.set(tmpRefMin);
		parameterMax.set(tmpRefMax);
		return true;
	}
	return false;
}

//--------------------------------------------------------------
bool ofxImGui::AddRange(const std::string& name, ofParameter<glm::vec3>& parameterMin, ofParameter<glm::vec3>& parameterMax, float speed)
{
	auto result = false;
	auto tmpRefMin = parameterMin.get();
	auto tmpRefMax = parameterMax.get();
	if (ImGui::DragFloatRange2(GetUniqueName(name + " X"), &tmpRefMin.x, &tmpRefMax.x, speed, parameterMin.getMin().x, parameterMax.getMax().x))
	{
		result |= true;
	}
	if (ImGui::DragFloatRange2(GetUniqueName(name + " Y"), &tmpRefMin.y, &tmpRefMax.y, speed, parameterMin.getMin().y, parameterMax.getMax().y))
	{
		result |= true;
	}
	if (ImGui::DragFloatRange2(GetUniqueName(name + " Z"), &tmpRefMin.z, &tmpRefMax.z, speed, parameterMin.getMin().z, parameterMax.getMax().z))
	{
		result |= true;
	}
	if (result)
	{
		parameterMin.set(tmpRefMin);
		parameterMax.set(tmpRefMax);
		return true;
	}
	return false;
}

//--------------------------------------------------------------
bool ofxImGui::AddRange(const std::string& name, ofParameter<glm::vec4>& parameterMin, ofParameter<glm::vec4>& parameterMax, float speed)
{
	auto result = false;
	auto tmpRefMin = parameterMin.get();
	auto tmpRefMax = parameterMax.get();
	if (ImGui::DragFloatRange2(GetUniqueName(name + " X"), &tmpRefMin.x, &tmpRefMax.x, speed, parameterMin.getMin().x, parameterMax.getMax().x))
	{
		result |= true;
	}
	if (ImGui::DragFloatRange2(GetUniqueName(name + " Y"), &tmpRefMin.y, &tmpRefMax.y, speed, parameterMin.getMin().y, parameterMax.getMax().y))
	{
		result |= true;
	}
	if (ImGui::DragFloatRange2(GetUniqueName(name + " Z"), &tmpRefMin.z, &tmpRefMax.z, speed, parameterMin.getMin().z, parameterMax.getMax().z))
	{
		result |= true;
	}
	if (ImGui::DragFloatRange2(GetUniqueName(name + " W"), &tmpRefMin.w, &tmpRefMax.w, speed, parameterMin.getMin().w, parameterMax.getMax().w))
	{
		result |= true;
	}
	if (result)
	{
		parameterMin.set(tmpRefMin);
		parameterMax.set(tmpRefMax);
		return true;
	}
	return false;
}

#endif

#if OF_VERSION_MINOR >= 10

//--------------------------------------------------------------
bool ofxImGui::AddValues(const std::string& name, std::vector<glm::tvec2<int>>& values, int minValue, int maxValue)
{
	auto result = false;
	for (int i = 0; i < values.size(); ++i)
	{
		const auto iname = name + " " + ofToString(i);
		result |= ImGui::SliderInt2(GetUniqueName(iname), glm::value_ptr(values[i]), minValue, maxValue);
	}
	return result;
}

//--------------------------------------------------------------
bool ofxImGui::AddValues(const std::string& name, std::vector<glm::tvec3<int>>& values, int minValue, int maxValue)
{
	auto result = false;
	for (int i = 0; i < values.size(); ++i)
	{
		const auto iname = name + " " + ofToString(i);
		result |= ImGui::SliderInt3(GetUniqueName(iname), glm::value_ptr(values[i]), minValue, maxValue);
	}
	return result;
}

//--------------------------------------------------------------
bool ofxImGui::AddValues(const std::string& name, std::vector<glm::tvec4<int>>& values, int minValue, int maxValue)
{
	auto result = false;
	for (int i = 0; i < values.size(); ++i)
	{
		const auto iname = name + " " + ofToString(i);
		result |= ImGui::SliderInt4(GetUniqueName(iname), glm::value_ptr(values[i]), minValue, maxValue);
	}
	return result;
}

//--------------------------------------------------------------
bool ofxImGui::AddValues(const std::string& name, std::vector<glm::vec2>& values, float minValue, float maxValue)
{
	auto result = false;
	for (int i = 0; i < values.size(); ++i)
	{
		const auto iname = name + " " + ofToString(i);
		result |= ImGui::SliderFloat2(GetUniqueName(iname), glm::value_ptr(values[i]), minValue, maxValue);
	}
	return result;
}

//--------------------------------------------------------------
bool ofxImGui::AddValues(const std::string& name, std::vector<glm::vec3>& values, float minValue, float maxValue)
{
	auto result = false;
	for (int i = 0; i < values.size(); ++i)
	{
		const auto iname = name + " " + ofToString(i);
		result |= ImGui::SliderFloat3(GetUniqueName(iname), glm::value_ptr(values[i]), minValue, maxValue);
	}
	return result;
}

//--------------------------------------------------------------
bool ofxImGui::AddValues(const std::string& name, std::vector<glm::vec4>& values, float minValue, float maxValue)
{
	auto result = false;
	for (int i = 0; i < values.size(); ++i)
	{
		const auto iname = name + " " + ofToString(i);
		result |= ImGui::SliderFloat4(GetUniqueName(iname), glm::value_ptr(values[i]), minValue, maxValue);
	}
	return result;
}

#endif

//--------------------------------------------------------------
bool ofxImGui::AddValues(const std::string& name, std::vector<ofVec2f>& values, float minValue, float maxValue)
{
	auto result = false;
	for (int i = 0; i < values.size(); ++i)
	{
		const auto iname = name + " " + ofToString(i);
		result |= ImGui::SliderFloat2(GetUniqueName(iname), values[i].getPtr(), minValue, maxValue);
	}
	return result;
}

//--------------------------------------------------------------
bool ofxImGui::AddValues(const std::string& name, std::vector<ofVec3f>& values, float minValue, float maxValue)
{
	auto result = false;
	for (int i = 0; i < values.size(); ++i)
	{
		const auto iname = name + " " + ofToString(i);
		result |= ImGui::SliderFloat3(GetUniqueName(iname), values[i].getPtr(), minValue, maxValue);
	}
	return result;
}

//--------------------------------------------------------------
bool ofxImGui::AddValues(const std::string& name, std::vector<ofVec4f>& values, float minValue, float maxValue)
{
	auto result = false;
	for (int i = 0; i < values.size(); ++i)
	{
		const auto iname = name + " " + ofToString(i);
		result |= ImGui::SliderFloat4(GetUniqueName(iname), values[i].getPtr(), minValue, maxValue);
	}
	return result;
}

//--------------------------------------------------------------
void ofxImGui::AddImage(ofBaseHasTexture& hasTexture, const ofVec2f& size)
{
	ofxImGui::AddImage(hasTexture.getTexture(), size);
}

//--------------------------------------------------------------
void ofxImGui::AddImage(ofTexture& texture, const ofVec2f& size)
{
	ImTextureID textureID = GetImTextureID(texture);
	ImGui::Image(textureID, size);
}


static auto vector_getter = [](void* vec, int idx, const char** out_text)
{
    auto& vector = *static_cast<std::vector<std::string>*>(vec);
    if (idx < 0 || idx >= static_cast<int>(vector.size())) { return false; }
    *out_text = vector.at(idx).c_str();
    return true;
};

bool ofxImGui::VectorCombo(const char* label, int* currIndex, std::vector<std::string>& values)
{
    if (values.empty()) { return false; }
    return ImGui::Combo(label, currIndex, vector_getter,
                        static_cast<void*>(&values), values.size());
}

bool ofxImGui::VectorListBox(const char* label, int* currIndex, std::vector<std::string>& values)
{
    if (values.empty()) { return false; }
    return ImGui::ListBox(label, currIndex, vector_getter,
                   static_cast<void*>(&values), values.size());
}

namespace ImGui {
    IMGUI_API bool Checkbox(ofParameter<bool>& v) {
        return Checkbox(v.getEscapedName().c_str(), &v);
    }
    IMGUI_API bool Checkbox(const char * label, ofParameter<bool>* v) {
        bool val = v->get();
        bool changed = Checkbox(label, &val);
        if (changed) v->set(val);
        return changed;
    }

    IMGUI_API bool CheckboxFlags(ofParameter<unsigned int>& flags, unsigned int flags_value) {
        return CheckboxFlags(flags.getEscapedName().c_str(), &flags, flags_value);
    }

    IMGUI_API bool CheckboxFlags(const char * label, ofParameter<unsigned int>* flags, unsigned int flags_value) {
        unsigned int val = flags->get();
        bool changed = CheckboxFlags(label, &val, flags_value);
        if (changed) flags->set(val);
        return changed;
    }

    IMGUI_API bool RadioButton(ofParameter<int>& v, int v_button) {
        return RadioButton(v.getEscapedName().c_str(), &v, v_button);
    }

    IMGUI_API bool RadioButton(const char * label, ofParameter<int>* v, int v_button) {
        int val = v->get();
        bool changed = RadioButton(label, &val, v_button);
        if (changed) v->set(val);
        return changed;
    }

    struct StringCB {
        std::string* str;
        ImGuiTextEditCallback chain;
        void* user_data;
        StringCB(std::string* _str, ImGuiTextEditCallback _chain, void* _user_data)
            : str(_str), chain(_chain), user_data(_user_data) { }
    };
    
    static int InputTextStdStringCallback(ImGuiInputTextCallbackData *data) {
        StringCB *cb_data = (StringCB *)data->UserData;
        if (data->EventFlag == ImGuiInputTextFlags_CallbackResize) {
            std::string* str = cb_data->str;
            IM_ASSERT(data->Buf == str->c_str());
            str->resize(data->BufTextLen);
            data->Buf = (char*)str->c_str();
            if (cb_data->chain) {
                data->UserData = cb_data->user_data;
                return cb_data->chain(data);
            }
        }
        else if (cb_data->chain) {
            data->UserData = cb_data->user_data;
            return cb_data->chain(data);
        }
        return 0;
    }
    struct ofParameterStringCB {
        ofParameter<std::string>* str;
        ImGuiTextEditCallback chain;
        void* user_data;
        ofParameterStringCB(ofParameter<std::string>* _str, ImGuiTextEditCallback _chain, void* _user_data)
            : str(_str), chain(_chain), user_data(_user_data) { }
    };

    static int InputTextOfParameterStringCallback(ImGuiInputTextCallbackData *data) {
        ofParameterStringCB *cb_data = (ofParameterStringCB *)data->UserData;
        if (data->EventFlag == ImGuiInputTextFlags_CallbackResize) {
            std::string *str = (std::string*)cb_data->str->getInternalObject();
            IM_ASSERT(data->Buf == str->c_str());
            str->resize(data->BufTextLen);
            data->Buf = (char*)str->c_str();
            if (cb_data->chain) {
                data->UserData = cb_data->user_data;
                return cb_data->chain(data);
            }
        }
        else if (cb_data->chain) {
            data->UserData = cb_data->user_data;
            return cb_data->chain(data);
        }
        return 0;
    }

    IMGUI_API bool InputText(const char * label, std::string * str, ImGuiInputTextFlags flags, ImGuiInputTextCallback callback, void * user_data) {
        StringCB cb_data(str, callback, user_data);
        return InputText(label, (char*)str->c_str(), str->capacity()+1, flags, InputTextStdStringCallback, (void*)&cb_data);
    }

    IMGUI_API bool InputTextMultiline(const char * label, std::string * str, ImVec2 const & size, ImGuiInputTextFlags flags, ImGuiInputTextCallback callback, void * user_data) {
        StringCB cb_data(str, callback, user_data);
        return InputTextMultiline(label, (char*)str->c_str(), str->capacity()+1, size, flags, InputTextStdStringCallback, (void*)&cb_data);
    }

    IMGUI_API bool InputText(ofParameter<std::string>& str, ImGuiInputTextFlags flags, ImGuiInputTextCallback callback, void * user_data) {
        return InputText(str.getEscapedName().c_str(), &str, flags, callback, user_data);
    }

    IMGUI_API bool InputTextMultiline(ofParameter<std::string>& str, ImVec2 const & size, ImGuiInputTextFlags flags, ImGuiInputTextCallback callback, void * user_data) {
        return InputTextMultiline(str.getEscapedName().c_str(), &str, size, flags, callback, user_data);
    }

    IMGUI_API bool InputText(const char * label, ofParameter<std::string>* str, ImGuiInputTextFlags flags, ImGuiInputTextCallback callback, void * user_data) {
        ofParameterStringCB cb_data(str, callback, user_data);
        return InputText(label, (char*)str->get().c_str(), str->get().capacity()+1, flags, InputTextOfParameterStringCallback, (void*)&cb_data);
    }

    IMGUI_API bool InputTextMultiline(const char * label, ofParameter<std::string>* str, ImVec2 const & size, ImGuiInputTextFlags flags, ImGuiInputTextCallback callback, void * user_data) {
        ofParameterStringCB cb_data(str, callback, user_data);
        return InputTextMultiline(label, (char*)str->get().c_str(), str->get().capacity()+1, size, flags, InputTextOfParameterStringCallback, (void*)&cb_data);
    }

    IMGUI_API bool DragFloat(ofParameter<float>& v, float v_speed, float v_min, float v_max, const char * format, float power) {
        return DragFloat(v.getEscapedName().c_str(), &v, v_speed, v_min, v_max, format, power);
    }
    IMGUI_API bool DragFloat(const char * label, ofParameter<float>* v, float v_speed, float v_min, float v_max, const char * format, float power) {
        float f = v->get();
        if (DragFloat(label, &f, v_speed, v_min, v_max, format, power)) {
            v->set(f);
            return true;
        }
        return false;
    }
    IMGUI_API bool DragFloat2(const char * label, ofVec2f * v, float v_speed, float v_min, float v_max, const char * format, float power) {
        return DragFloat2(label, v->getPtr(), v_speed, v_min, v_max, format, power);
    }
    IMGUI_API bool DragFloat2(ofParameter<ofVec2f>& v, float v_speed, float v_min, float v_max, const char * format, float power) {
        return DragFloat2(v.getEscapedName().c_str(), &v, v_speed, v_min, v_max, format, power);
    }
    IMGUI_API bool DragFloat2(const char * label, ofParameter<ofVec2f>* v, float v_speed, float v_min, float v_max, const char * format, float power) {
        ofVec2f vec = v->get();
        if (DragFloat2(label, vec.getPtr(), v_speed, v_min, v_max, format, power)) {
            v->set(vec);
            return true;
        }
        return false;
    }
    IMGUI_API bool DragFloat3(const char * label, ofVec3f * v, float v_speed, float v_min, float v_max, const char * format, float power) {
        return DragFloat3(label, v->getPtr(), v_speed, v_min, v_max, format, power);
    }
    IMGUI_API bool DragFloat3(ofParameter<ofVec3f>& v, float v_speed, float v_min, float v_max, const char * format, float power) {
        return DragFloat3(v.getEscapedName().c_str(), &v, v_speed, v_min, v_max, format, power);
    }
    IMGUI_API bool DragFloat3(const char * label, ofParameter<ofVec3f>* v, float v_speed, float v_min, float v_max, const char * format, float power) {
        ofVec3f vec = v->get();
        if (DragFloat3(label, vec.getPtr(), v_speed, v_min, v_max, format, power)) {
            v->set(vec);
            return true;
        }
        return false;
    }
    IMGUI_API bool DragFloat4(const char * label, ofVec4f * v, float v_speed, float v_min, float v_max, const char * format, float power) {
        return DragFloat4(label, v->getPtr(), v_speed, v_min, v_max, format, power);
    }
    IMGUI_API bool DragFloat4(ofParameter<ofVec4f>& v, float v_speed, float v_min, float v_max, const char * format, float power) {
        return DragFloat4(v.getEscapedName().c_str(), &v, v_speed, v_min, v_max, format, power);
    }
    IMGUI_API bool DragFloat4(const char * label, ofParameter<ofVec4f>* v, float v_speed, float v_min, float v_max, const char * format, float power) {
        ofVec4f vec = v->get();
        if (DragFloat4(label, vec.getPtr(), v_speed, v_min, v_max, format, power)) {
            v->set(vec);
            return true;
        }
        return false;
    }
    IMGUI_API bool DragFloatRange2(const char * label, ofParameter<float>* v_current_min, ofParameter<float>* v_current_max, float v_speed, float v_min, float v_max, const char * format, const char * format_max, float power) {
        float min = v_current_min->get(), max = v_current_max->get();
        if (DragFloatRange2(label, &min, &max, v_speed, v_min, v_max, format, format_max, power)) {
            v_current_min->set(min);
            v_current_max->set(max);
            return true;
        }
        return false;
    }
    IMGUI_API bool DragInt(ofParameter<int>& v, float v_speed, int v_min, int v_max, const char * format) {
        return DragInt(v.getEscapedName().c_str(), &v, v_speed, v_min, v_max, format);
    }
    IMGUI_API bool DragInt(const char * label, ofParameter<int>* v, float v_speed, int v_min, int v_max, const char * format) {
        int i = v->get();
        if (DragInt(label, &i, v_speed, v_min, v_max, format)) {
            v->set(i);
            return true;
        }
        return false;
    }
    IMGUI_API bool DragIntRange2(const char * label, ofParameter<int>* v_current_min, ofParameter<int>* v_current_max, float v_speed, int v_min, int v_max, const char * format, const char * format_max) {
        int current_min = v_current_min->get(), current_max = v_current_max->get();
        if (DragIntRange2(label, &current_min, &current_max, v_speed, v_min, v_max, format, format_max)) {
            v_current_min->set(current_min);
            v_current_max->set(current_max);
            return true;
        }
        return false;
    }
    IMGUI_API bool SliderFloat(ofParameter<float>& v, float v_min, float v_max, const char * format, float power) {
        return SliderFloat(v.getEscapedName().c_str(), &v, v_min, v_max, format, power);
    }
    IMGUI_API bool SliderFloat(const char * label, ofParameter<float>* v, float v_min, float v_max, const char * format, float power) {
        float f = v->get();
        if (SliderFloat(label, &f, v_min, v_max, format, power)) {
            v->set(f);
            return true;
        }
        return false;
    }

    IMGUI_API bool SliderFloat2(const char * label, ofVec2f * v, float v_min, float v_max, const char * format, float power) {
        return SliderFloat2(label, v->getPtr(), v_min, v_max, format, power);
    }

    IMGUI_API bool SliderFloat2(ofParameter<ofVec2f>& v, float v_min, float v_max, const char * format, float power) {
        return SliderFloat2(v.getEscapedName().c_str(), &v, v_min, v_max, format, power);
    }
    IMGUI_API bool SliderFloat2(const char * label, ofParameter<ofVec2f>* v, float v_min, float v_max, const char * format, float power) {
        ofVec2f f = v->get();
        if (SliderFloat2(label, f.getPtr(), v_min, v_max, format, power)) {
            v->set(f);
            return true;
        }
        return false;
    }

    IMGUI_API bool SliderFloat3(const char * label, ofVec3f * v, float v_min, float v_max, const char * format, float power) {
        return SliderFloat3(label, v->getPtr(), v_min, v_max, format, power);
    }
    IMGUI_API bool SliderFloat3(ofParameter<ofVec3f>& v, float v_min, float v_max, const char * format, float power) {
        return SliderFloat3(v.getEscapedName().c_str(), &v, v_min, v_max, format, power);
    }
    IMGUI_API bool SliderFloat3(const char * label, ofParameter<ofVec3f>* v, float v_min, float v_max, const char * format, float power) {
        ofVec3f f = v->get();
        if (SliderFloat3(label, f.getPtr(), v_min, v_max, format, power)) {
            v->set(f);
            return true;
        }
        return false;
    }

    IMGUI_API bool SliderFloat4(const char * label, ofVec4f * v, float v_min, float v_max, const char * format, float power) {
        return SliderFloat4(label, v->getPtr(), v_min, v_max, format, power);
    }
    IMGUI_API bool SliderFloat4(ofParameter<ofVec4f>& v, float v_min, float v_max, const char * format, float power) {
        return SliderFloat4(v.getEscapedName().c_str(), &v, v_min, v_max, format, power);
    }
    IMGUI_API bool SliderFloat4(const char * label, ofParameter<ofVec4f>* v, float v_min, float v_max, const char * format, float power) {
        ofVec4f f = v->get();
        if (SliderFloat4(label, f.getPtr(), v_min, v_max, format, power)) {
            v->set(f);
            return true;
        }
        return false;
    }
    IMGUI_API bool SliderAngle(ofParameter<float>& v_rad, float v_degrees_min, float v_degrees_max) {
        return SliderAngle(v_rad.getEscapedName().c_str(), &v_rad, v_degrees_min, v_degrees_max);
    }
    IMGUI_API bool SliderAngle(const char * label, ofParameter<float>* v_rad, float v_degrees_min, float v_degrees_max) {
        if (SliderAngle(label, (float*)v_rad->getInternalObject(), v_degrees_min, v_degrees_max)) {
            v_rad->set(v_rad->get());
            return true;
        }
        return true;
    }
    IMGUI_API bool SliderInt(ofParameter<int>& v, int v_min, int v_max, const char * format) {
        return SliderInt(v.getEscapedName().c_str(), &v, v_min, v_max, format);
    }
    IMGUI_API bool SliderInt(const char * label, ofParameter<int>* v, int v_min, int v_max, const char * format) {
        if (SliderInt(label, (int*)v->getInternalObject(), v_min, v_max, format)) {
            v->set(v->get());
            return true;
        }
        return false;
    }
    IMGUI_API bool VSliderFloat(const ImVec2 & size, ofParameter<float>& v, float v_min, float v_max, const char * format, float power) {
        return VSliderFloat(v.getEscapedName().c_str(), size, &v, v_min, v_max, format, power);
    }
    IMGUI_API bool VSliderFloat(const char * label, const ImVec2 & size, ofParameter<float>* v, float v_min, float v_max, const char * format, float power) {
        if (VSliderFloat(label, size, (float*)v->getInternalObject(), v_min, v_max, format, power)) {
            v->set(v->get());
            return true;
        }
        return false;
    }
    IMGUI_API bool VSliderInt(const ImVec2 & size, ofParameter<int>& v, int v_min, int v_max, const char * format) {
        return VSliderInt(v.getEscapedName().c_str(), size, &v, v_min, v_max, format);
    }
    IMGUI_API bool VSliderInt(const char * label, const ImVec2 & size, ofParameter<int>* v, int v_min, int v_max, const char * format) {
        if (VSliderInt(label, size, (int*)v->getInternalObject(), v_min, v_max, format)) {
            v->set(v->get());
            return true;
        }
        return false;
    }
    IMGUI_API bool ColorEdit3(const char * label, ofColor * color, ImGuiColorEditFlags flags) {
        float temp[3] = { (float)color->r, (float)color->g, (float)color->b };
        if (ColorEdit3(label, temp, flags)) {
            color->set(temp[0], temp[1], temp[2]);
            return true;
        }
        return false;
    }
    IMGUI_API bool ColorEdit3(const char * label, ofFloatColor * color, ImGuiColorEditFlags flags) {
        float temp[3] = { color->r, color->g, color->b };
        if (ColorEdit3(label, temp, flags)) {
            color->set(temp[0], temp[1], temp[2]);
            return true;
        }
        return false;
    }
    IMGUI_API bool ColorEdit4(const char * label, ofColor * color, ImGuiColorEditFlags flags) {
        float temp[4] = { (float)color->r, (float)color->g, (float)color->b, (float)color->a };
        if (ColorEdit4(label, temp, flags)) {
            color->set(temp[0], temp[1], temp[2], temp[3]);
            return true;
        }
        return false;
    }
    IMGUI_API bool ColorEdit4(const char * label, ofFloatColor * color, ImGuiColorEditFlags flags) {
        float temp[4] = { color->r, color->g, color->b, color->a };
        if (ColorEdit4(label, temp, flags)) {
            color->set(temp[0], temp[1], temp[2], temp[3]);
            return true;
        }
        return false;
    }
    IMGUI_API bool ColorPicker3(const char * label, ofColor * color, ImGuiColorEditFlags flags) {
        float temp[3] = { (float)color->r, (float)color->g, (float)color->b };
        if (ColorPicker3(label, temp, flags)) {
            color->set(temp[0], temp[1], temp[2]);
            return true;
        }
        return false;
    }
    IMGUI_API bool ColorPicker3(const char * label, ofFloatColor * color, ImGuiColorEditFlags flags) {
        float temp[3] = { color->r, color->g, color->b };
        if (ColorPicker3(label, temp, flags)) {
            color->set(temp[0], temp[1], temp[2]);
            return true;
        }
        return false;
    }
    IMGUI_API bool ColorPicker4(const char * label, ofColor * color, ImGuiColorEditFlags flags, const ofColor * ref_col) {
        float temp[4] = { (float)color->r, (float)color->g, (float)color->b, (float)color->a };
        if (ref_col) {
            float ref[4] = { (float)ref_col->r, (float)ref_col->g, (float)ref_col->b, (float)ref_col->a };
            if (ColorPicker4(label, temp, flags, ref)) {
                color->set(temp[0], temp[1], temp[2], temp[3]);
                return true;
            }
        } else if (ColorPicker4(label, temp, flags, nullptr)) {
            color->set(temp[0], temp[1], temp[2], temp[3]);
            return true;
        }
        return false;
    }
    IMGUI_API bool ColorPicker4(const char * label, ofFloatColor * color, ImGuiColorEditFlags flags, const ofFloatColor * ref_col) {
        float temp[4] = { color->r, color->g, color->b, color->a };
        if (ref_col) {
            float ref[4] = { ref_col->r, ref_col->g, ref_col->b, ref_col->a };
            if (ColorPicker4(label, temp, flags, ref)) {
                color->set(temp[0], temp[1], temp[2], temp[3]);
                return true;
            }
        } else if (ColorPicker4(label, temp, flags, nullptr)) {
            color->set(temp[0], temp[1], temp[2], temp[3]);
            return true;
        }
        return false;
    }
    IMGUI_API bool ColorButton(const char * desc_id, ofColor const & col, ImGuiColorEditFlags flags, ImVec2 size) {
        return ColorButton(desc_id, ImVec4(col.r, col.g, col.b, col.a), flags, size);
    }
    IMGUI_API bool ColorButton(const char * desc_id, ofFloatColor const & col, ImGuiColorEditFlags flags, ImVec2 size) {
        return ColorButton(desc_id, ImVec4(col.r, col.g, col.b, col.a), flags, size);
    }
    IMGUI_API void Text(std::string const & text) {
        TextUnformatted(text.c_str());
    }
    IMGUI_API void TextColored(ImVec4 const & col, std::string const & text) {
        TextColored(col, "%s", text.c_str());
    }
    IMGUI_API void TextDisabled(std::string const & text) {
        TextDisabled("%s", text.c_str());
    }
    IMGUI_API void TextWrapped(std::string const & text) {
        TextWrapped("%s", text.c_str());
    }
    IMGUI_API void LabelText(const char * label, std::string const & text) {
        LabelText(label, "%s", text.c_str());
    }
    IMGUI_API void BulletText(std::string const & text) {
        BulletText("%s", text.c_str());
    }
    IMGUI_API void Image(const ofBaseHasTexture& image, const ImVec2& size, const ImVec2 & uv0, const ImVec2 & uv1, const ImVec4 & tint_col, const ImVec4 & border_col) {
        Image(image.getTexture(), size, uv0, uv1, tint_col, border_col);
    }
    IMGUI_API bool ImageButton(const ofBaseHasTexture& image, const ImVec2& size, const ImVec2 & uv0, const ImVec2 & uv1, int frame_padding, const ImVec4 & bg_col, const ImVec4 & tint_col) {
        return ImageButton(image.getTexture(), size, uv0, uv1, frame_padding, bg_col, tint_col);
    }
    IMGUI_API void Image(const ofTexture& texture, const ImVec2 & size, const ImVec2 & uv0, const ImVec2 & uv1, const ImVec4 & tint_col, const ImVec4 & border_col) {
        Image((ImTextureID)texture.getTextureData().textureID, size, uv0, uv1, tint_col, border_col);
    }
    IMGUI_API bool ImageButton(const ofTexture& texture, const ImVec2 & size, const ImVec2 & uv0, const ImVec2 & uv1, int frame_padding, const ImVec4 & bg_col, const ImVec4 & tint_col) {
        return ImageButton((ImTextureID)texture.getTextureData().textureID, size, uv0, uv1, frame_padding, bg_col, tint_col);
    }
}
