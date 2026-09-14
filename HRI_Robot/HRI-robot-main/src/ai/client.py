import sys
import subprocess
import pyttsx3
import speech_recognition as sr
from google import genai
from reachy_mini import ReachyMini
from reachy_mini.utils import create_head_pose
from dotenv import load_dotenv
import time
import os

load_dotenv()
api_key = os.getenv("GEMINI_API_KEY")

recognizer = sr.Recognizer()

# Set GEMINI_API_KEY as an environment variable before running
client = genai.Client()
# this will start a new chat that handles history automatically

generation_config = {
  "response_mime_type": "application/json",
  "response_schema": {
    "type": "object",
    "properties": {
      "user_message": {"type": "string", "description": "A message to speak to the user based on the user input and current conversation state"},
      "internal_rating": {"type": "integer", "description": "A score from 1-10 based on user mental health"},
      "suggested_gestures": {"type": "string", "description": "A suggestion for the best gesture to take based on the user_message and internal_rating. You should only choose one. "
      "The gestures you can choose from are:['amazed1', 'anxiety1', 'attentive1', 'attentive2', 'boredom1', 'boredom2', 'calming1', 'cheerful1', 'come1', 'confused1', 'contempt1', 'curious1', 'dance1', 'dance2', 'dance3', 'disgusted1', 'displeased1', 'displeased2', 'downcast1', 'dying1', 'electric1', 'enthusiastic1', 'enthusiastic2', 'exhausted1', 'fear1', 'frustrated1', 'furious1', 'go_away1', 'grateful1', 'helpful1', 'helpful2', 'impatient1', 'impatient2', 'incomprehensible2', 'indifferent1', 'inquiring1', 'inquiring2', 'inquiring3', 'irritated1', 'irritated2', 'laughing1', 'laughing2', 'lonely1', 'lost1', 'loving1', 'no1', 'no_excited1', 'no_sad1', 'oops1', 'oops2', 'proud1', 'proud2', 'proud3', 'rage1', 'relief1', 'relief2', 'reprimand1', 'reprimand2', 'reprimand3', 'resigned1', 'sad1', 'sad2', 'scared1', 'serenity1', 'shy1', 'sleep1', 'success1', 'success2', 'surprised1', 'surprised2', 'thoughtful1', 'thoughtful2', 'tired1', 'uncertain1', 'uncomfortable1', 'understanding1', 'understanding2', 'welcoming1', 'welcoming2', 'yes1', 'yes_sad1'] "}
    },
    "required": ["user_message", "internal_rating", "suggested_gestures"]
  }
}

chat = client.chats.create(model="gemini-3.1-flash-lite", config=generation_config)

def safe_send(chat, message, retries=4):
    for i in range(retries):
        try:
            return chat.send_message(message).text
        except Exception as e:
            if "503" in str(e):
                wait = 15  # exponential backoff
                print(f"Server busy, retrying in {wait}s...")
                time.sleep(wait)
            else:
                raise e
    return "Error: API unavailable after retries."