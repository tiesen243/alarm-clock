onerror {resume}
quietly WaveActivateNextPane {} 0
add wave -noupdate /counter_4_bits_tb/COUNTER4BITS/iClk
add wave -noupdate /counter_4_bits_tb/COUNTER4BITS/iReset_n
add wave -noupdate /counter_4_bits_tb/COUNTER4BITS/iEnable
add wave -noupdate /counter_4_bits_tb/COUNTER4BITS/iClear
add wave -noupdate /counter_4_bits_tb/COUNTER4BITS/iUp_down
add wave -noupdate -expand /counter_4_bits_tb/COUNTER4BITS/iInitialValue
add wave -noupdate -expand /counter_4_bits_tb/COUNTER4BITS/oData
TreeUpdate [SetDefaultTree]
WaveRestoreCursors {{Cursor 1} {212 ps} 0}
quietly wave cursor active 1
configure wave -namecolwidth 150
configure wave -valuecolwidth 100
configure wave -justifyvalue left
configure wave -signalnamewidth 0
configure wave -snapdistance 10
configure wave -datasetprefix 0
configure wave -rowmargin 4
configure wave -childrowmargin 2
configure wave -gridoffset 0
configure wave -gridperiod 1
configure wave -griddelta 40
configure wave -timeline 0
configure wave -timelineunits ps
update
WaveRestoreZoom {0 ps} {1962 ps}
