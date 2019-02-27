#include "InputDiff.hpp"

InputDiff::InputDiff() : timeDiff(0), singleDiffs{} {}

void InputDiff::initialize(uint16_t timeDiff, Gamecube_Report_t &firstReport,
                           Gamecube_Report_t &secondReport) {
    this->timeDiff = timeDiff;
    resetDiffs();
    storeDiffs(firstReport, secondReport);
}

void InputDiff::storeDiffs(Gamecube_Report_t &firstReport,
                           Gamecube_Report_t &secondReport) {
    uint8_t firstReportInputs[] = {
        firstReport.a,     firstReport.b,     firstReport.x,
        firstReport.y,     firstReport.z,     firstReport.l,
        firstReport.left,  firstReport.r,     firstReport.right,
        firstReport.xAxis, firstReport.yAxis, firstReport.cxAxis,
        firstReport.cyAxis};
    uint8_t secondReportInputs[] = {
        secondReport.a,     secondReport.b,     secondReport.x,
        secondReport.y,     secondReport.z,     secondReport.l,
        secondReport.left,  secondReport.r,     secondReport.right,
        secondReport.xAxis, secondReport.yAxis, secondReport.cxAxis,
        secondReport.cyAxis};
    ControllerInput associatedInputs[] = {
        ControllerInput::A,        ControllerInput::B,
        ControllerInput::X,        ControllerInput::Y,
        ControllerInput::Z,        ControllerInput::L,
        ControllerInput::L_ANALOG, ControllerInput::R,
        ControllerInput::R_ANALOG, ControllerInput::XAXIS,
        ControllerInput::YAXIS,    ControllerInput::C_XAXIS,
        ControllerInput::C_YAXIS};
    for (int i = 0; i < MAX_CONS_INPUTS; i++) {
        if (inputsDiffer(firstReportInputs[i], secondReportInputs[i],
                         associatedInputs[i])) {
            storeSingleDiff(firstReportInputs[i], secondReportInputs[i],
                            associatedInputs[i]);
        }
    }
}

bool InputDiff::inputsDiffer(uint8_t first, uint8_t second,
                             ControllerInput input) {
    if (input == ControllerInput::L_ANALOG ||
        input == ControllerInput::R_ANALOG || input == ControllerInput::XAXIS ||
        input == ControllerInput::YAXIS || input == ControllerInput::C_XAXIS ||
        input == ControllerInput::C_YAXIS) {
        // axis inputs should only be counted if they are greater than the
        // allowable drift
        return abs(second - first) > ALLOWABLE_AXIS_DRIFT;
    }
    return second - first > 0;
}

/**
 * Store dummy diffs in each element
 */
void InputDiff::resetDiffs() {
    for (int i = 0; i < MAX_CONS_INPUTS; i++) {
        singleDiffs[i].input = ControllerInput::A;
        singleDiffs[i].valueDiff = 0;
    }
}

void InputDiff::storeSingleDiff(uint8_t firstVal, uint8_t secondVal,
                                ControllerInput input) {
    int16_t diff = (int16_t)firstVal - (int16_t)secondVal;
    this->singleDiffs[(int)input] = (SingleInputDiff){input, diff};
}

uint16_t InputDiff::getTimeDiff() { return timeDiff; }

void InputDiff::applyTo(Gamecube_Data_t &dataToModify) {
    for (int i = 0; i < MAX_CONS_INPUTS; i++) {
        applySingleDiffTo(singleDiffs[i], dataToModify);
    }
}

void InputDiff::applySingleDiffTo(SingleInputDiff &singleInputDiff,
                                  Gamecube_Data_t &dataToModify) {
    switch (singleInputDiff.input) {
    case ControllerInput::A:
        dataToModify.report.a += singleInputDiff.valueDiff;
        break;
    case ControllerInput::B:
        dataToModify.report.b += singleInputDiff.valueDiff;
        break;
    case ControllerInput::X:
        dataToModify.report.x += singleInputDiff.valueDiff;
        break;
    case ControllerInput::Y:
        dataToModify.report.y += singleInputDiff.valueDiff;
        break;
    case ControllerInput::Z:
        dataToModify.report.z += singleInputDiff.valueDiff;
        break;
    case ControllerInput::L:
        dataToModify.report.l += singleInputDiff.valueDiff;
        break;
    case ControllerInput::L_ANALOG:
        dataToModify.report.left += singleInputDiff.valueDiff;
        break;
    case ControllerInput::R:
        dataToModify.report.r += singleInputDiff.valueDiff;
        break;
    case ControllerInput::R_ANALOG:
        dataToModify.report.right += singleInputDiff.valueDiff;
        break;
    case ControllerInput::XAXIS:
        dataToModify.report.xAxis += singleInputDiff.valueDiff;
        break;
    case ControllerInput::YAXIS:
        dataToModify.report.yAxis += singleInputDiff.valueDiff;
        break;
    case ControllerInput::C_XAXIS:
        dataToModify.report.cxAxis += singleInputDiff.valueDiff;
        break;
    case ControllerInput::C_YAXIS:
        dataToModify.report.cyAxis += singleInputDiff.valueDiff;
        break;
    default:
#ifdef DEBUG
        Serial.print(F("Invalid controller input!"));
#endif
        break;
    }
}