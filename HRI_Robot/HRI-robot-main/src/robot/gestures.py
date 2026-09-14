#gester, mer abstrakta, vilka känslor
from reachy_mini.motion.recorded_move import RecordedMoves
from reachy_mini import ReachyMini
from reachy_mini.utils import create_head_pose
import asyncio
import subprocess
import platform
import time

# Uses manual movement instead of premade expressions, allows the robot to hold the expression for a predetermined amount of time.
def listen_expression(mini):
    mini.goto_target(
        head=create_head_pose(roll=10, degrees=True),
        antennas=[-1.0, 1.0],  # tilt to the side
        duration=1.0
    )
    time.sleep(8.0)

    mini.goto_target(
        head=create_head_pose(roll=0, degrees=True),
        antennas=[0.0, 0.0], 
        duration=0.5)
