{% set target  = params.BED_TEMP|int %}
{% set current = printer.heater_bed.temperature %}

{% if current < target - 20 %}
G4 P{ 5 * 60 * 1000 }       #Milliseconds to dwell
{% else %}
G4 P{ 1 * 60 * 1000 }
{% endif %}
G92 E0               # Reset Extruder
G1 Z2.0 F3000        # Move Z Axis to travel height
ACCEPT
SET_PIN PIN=my_led VALUE=1
G4 P2000
SET_PIN PIN=my_led VALUE=0
