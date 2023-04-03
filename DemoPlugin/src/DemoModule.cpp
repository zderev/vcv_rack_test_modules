#include "plugin.hpp"


struct DemoModule : Module {
	enum ParamId {
        FREQ_PARAM,
        CV_PARAM,
		PARAMS_LEN
	};
	enum InputId {
		CV_INPUT,
		INPUTS_LEN
	};
	enum OutputId {
		CV_OUTPUT,
		OUTPUTS_LEN
	};
	enum LightId {
		BLINK_LIGHT,
		LIGHTS_LEN
	};

	DemoModule() {
		config(PARAMS_LEN, INPUTS_LEN, OUTPUTS_LEN, LIGHTS_LEN);
        configParam(FREQ_PARAM, 0.f, 10.f, 0.f, string::f("Freq Master OSC"), "%", 0, 10);
        configParam(CV_PARAM, -1.f, 1.f, 0.f, string::f("CV Master OSC"), "%", 0, 100);
        configInput(CV_INPUT, "");
        configOutput(CV_OUTPUT, "");
	}
    float out;
	void process(const ProcessArgs& args) override {

        out = 0.f;

        float freq = params[FREQ_PARAM].getValue();
        float gain = params[CV_PARAM].getValue();
        float cvInput = inputs[CV_INPUT].getVoltage();

        out+=freq + gain*cvInput;

        outputs[CV_OUTPUT].setVoltage(out);
	}
};


struct DemoModuleWidget : ModuleWidget {
	DemoModuleWidget(DemoModule* module) {
        this->module = dynamic_cast<DemoModule*>(this->module);;
		setModule(module);
		setPanel(createPanel(asset::plugin(pluginInstance, "res/DemoModule.svg")));

		addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, 0)));
		addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, 0)));
		addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));
		addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));

		addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(7.548, 25.39)), module, DemoModule::FREQ_PARAM));

        addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(7.977, 44.105)), module, DemoModule::CV_PARAM));

		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(7.806, 64.005)), module, DemoModule::CV_OUTPUT));

        addInput(createInputCentered<PJ301MPort>(mm2px(Vec(8.063, 111.509)), module, DemoModule::CV_INPUT));
	}
};


Model* modelDemoModule = createModel<DemoModule, DemoModuleWidget>("DemoModule");

/**************************************************/
// EXPANDER CODE - starting here
/**************************************************/

#include "plugin.hpp"
struct DemoExpRModule : Module {

    enum ParamId  {PARAMS_LEN};
    enum InputId  {INPUTS_LEN};
    enum OutputId {CV_OUTPUT,OUTPUTS_LEN};
    enum LightId  {LIGHTS_LEN};

    DemoExpRModule() {
        config(PARAMS_LEN, INPUTS_LEN, OUTPUTS_LEN, LIGHTS_LEN);
        configOutput(CV_OUTPUT, "CV Output");
    }

    DemoModule* findHostModulePtr(Module* module) {
        if (module) {
            if (module->leftExpander.module) {
                // if it's the mother module, we're done
                if (module->leftExpander.module->model == modelDemoModule) {
                    return reinterpret_cast<DemoModule*>(module->leftExpander.module);
                }
                else if (module->leftExpander.module->model == modelDemoExpRModule) {
                    return findHostModulePtr(module->leftExpander.module);
                }
            }
        }
        return nullptr;
    }

    void process(const ProcessArgs& args) override {
        DemoModule const* mother = findHostModulePtr(this);
        float cvOut = 0.f;
        if (mother) {
            cvOut += mother->out;
        }
        outputs[CV_OUTPUT].setVoltage(cvOut);
    }
};

struct DemoExpRModuleWidget : ModuleWidget {

    DemoExpRModule* module;
    DemoExpRModuleWidget(DemoExpRModule* module) {

        this->module = module;
        setModule(module);
        setPanel(createPanel(asset::plugin(pluginInstance, "res/DemoModuleExpr.svg")));

        addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(5.08, 30.48)), module, DemoExpRModule::CV_OUTPUT));
    }

};

Model* modelDemoExpRModule = createModel<DemoExpRModule, DemoExpRModuleWidget>("DemoExpRModule");