#include <Arduino.h>
#include "presets.h"
#include "effects.h"
#include "ffthsv.h"

//triger on_burst dla calego wyswietlacza?
//dodanie pliku h z efektami,sterowanie przejsciami - wybrany efekt, odtwarzaj z kolejki, itp.s

effect_handler_t fire_effect_handlers[] = {fire_effect};


void setup() {
    Serial.begin(115200);
    FFTHSV_begin();
    FFTHSV_set_effects(fire_effect_handlers, 1);
    FFTHSV_select_effect(0);
}


void loop() {
    FFTHSV_update();
}
