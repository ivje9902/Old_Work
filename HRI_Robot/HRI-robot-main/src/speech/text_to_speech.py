import sys
import subprocess
import pyttsx3

# --- SPEAK FUNCTION ---
def speak(text, mini=None):
    print(f"[Robot]: {text}")
    if mini:
        mini.goto_target(antennas=[0.3, -0.3], duration=0.2)
    if sys.platform == "darwin":
        subprocess.run(["say", "-v", "Samantha", "-r", "160", text])
    else:
        engine = pyttsx3.init()
        engine.setProperty('rate', 160)
        engine.say(text)
        engine.runAndWait()
    if mini:
        mini.goto_target(antennas=[0.0, 0.0], duration=0.2)