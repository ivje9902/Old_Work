import speech_recognition as sr
import threading
from robot.gestures import listen_expression

# recognizer used for recording audio
recognizer = sr.Recognizer()

# --- LISTEN FUNCTION ---

def listen(mini=None):
    """Record from microphone and return recognised text, or empty string on failure. Plays emotion in parallel if mini is given."""
    emotion_thread = None
    try:
        if mini is not None:
            emotion_thread = threading.Thread(target=listen_expression, args=(mini,), daemon=True)
            emotion_thread.start()
        with sr.Microphone() as source:
            recognizer.adjust_for_ambient_noise(source)
            print("[Listening...]")
            audio = recognizer.listen(source, timeout=8)
        text = recognizer.recognize_google(audio)
        print(f"[User]: {text}")
        return text
    except sr.UnknownValueError:
        return ""
    except sr.RequestError:
        print("Speech recognition service unavailable.")
        return ""
    except sr.WaitTimeoutError:
        return ""
    finally:
        if emotion_thread is not None:
            emotion_thread.join(timeout=0.5)
    
