import sys
import threading
import subprocess
import pyttsx3
import time
import json
import speech_recognition as sr

from reachy_mini import ReachyMini
from reachy_mini.utils import create_head_pose

from robot.reachy_controller import play_emotion
from speech.speech_to_text import listen
from speech.text_to_speech import speak
from ai.client import chat, safe_send
from ai.promts import topic_list





with ReachyMini(media_backend="no_media") as mini:
    speak("Hello!, I’m Reachy Mini, and I’m here to chat with you today about how you’ve been feeling mentally and emotionally. This short conversation is a safe space for you to share about your day, your thoughts, and your stress levels", mini=mini)
    speak("How are you today?", mini=mini)
    score_list = []
    topic_idx = 0


    while topic_idx < len(topic_list):
        current_topic = topic_list[topic_idx]
        next_topic = topic_list[topic_idx + 1] if topic_idx + 1 < len(topic_list) else None
        user_input = listen(mini=mini)
        prompt = f"[Current topic: {current_topic}]\nUser said: {user_input}\n [Next topic: {next_topic}]"
        if not user_input:
            speak("Sorry I didn't catch that.")
            continue
        if "goodbye" in user_input.lower():
            speak("Goodbye!")
            break

        # Nod while "thinking"
        mini.goto_target(head=create_head_pose(z=5, mm=True), duration=0.3)
        print(user_input)
        reply = safe_send(chat, prompt)
        data = json.loads(reply)

        tasks = [
                    {
                        "func": play_emotion, 
                        "args": (mini, data.get("suggested_gestures"))
                    },
                    {
                        "func": speak, 
                        "args": (data.get("user_message"),), # Note the comma for a single-element tuple
                        "kwargs": {"mini": mini}
                    }
                ]

        threads = []

        # 2. Properly initialize threads
        for task in tasks:
            t = threading.Thread(
                target=task["func"], 
                args=task.get("args", ()), 
                kwargs=task.get("kwargs", {})
            )
            threads.append(t)
            t.start()

        # 3. Wait for both to finish
        for t in threads:
            t.join()
     #   speak(data.get("user_message"), mini=mini)
        score_list.append(data.get("internal_rating"))
        print(data.get("internal_rating"))
        print(data.get("suggested_gestures"))
       # play_emotion(mini, data.get("suggested_gestures"))


        # Reset head
        mini.goto_target(head=create_head_pose(), duration=0.3)

        topic_idx += 1

