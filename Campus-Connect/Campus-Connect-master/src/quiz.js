import { initializeApp } from "firebase/app";
import { getFirestore, collection, addDoc, getDocs, query, where, doc, setDoc, updateDoc, deleteDoc, count, getDoc, arrayUnion } from "firebase/firestore";

import { db, getCurrentUser } from './backend.js';


// DOM elements

//TODO: Documentation

const questionElem = document.getElementById("question");
const answerButtons = document.getElementById("answer-buttons");
const nextButton = document.getElementById("next-btn");


/**
 * Asynchronously creates a new quiz document in the database if a quiz with the same name doesn't exist already.
 * 
 * @async
 * @function createQuiz
 * @returns {Promise<void>} A promise that resolves once the quiz is created of if it already exists, resolves without creating a duplicate.
 * @throws {Error} If there is an error creating or checking for the quiz in the database.
 */
async function createQuiz() {
    var courseID = localStorage.getItem("ID");
    var quizName = document.getElementById("quiz-name").value;

    //Reference to the quizzes collection in the databse
    const docRef = doc(db, courseID, "Quizzes");
    const docSnap = collection(docRef, "all-quizzes");
    const docs = await getDocs(docSnap);

    let quizExists = false;
    docs.forEach((doc) => {
        if (doc.id === quizName) {
            quizExists = true;
        }
    });

    var errorMessageElement = document.getElementById("error-message");

    if (quizExists) {
        errorMessageElement.textContent = "A quiz with this name already exists. Please choose another name.";
    } else {
        errorMessageElement.textContent = "";
        errorMessageElement.hidden;
        const quizDocRef = doc(db, courseID, 'Quizzes', 'all-quizzes', quizName);
        await setDoc(quizDocRef, {
            questions: [],
            likes: 0
        });
        $('#questions-container').show();
        $('#quiz-name').hide();
        $('#submit-name-btn').hide();
        console.log("Document successfully created");

        const user = await getCurrentUser();
        const uid = user.uid;

        const pathString = `${courseID}/Quizzes/all-quizzes/${quizName}`;

        const userDocRef = doc(db, 'users', uid);
        const updateData = {
            files: {
                [quizName]: pathString
            }
        };

        await setDoc(userDocRef, updateData, { merge: true });
    }
}


/**
 * Asynchronously adds quiz questions to an existing quiz document in the database.
 * 
 * @async
 * @function addQuizQuestions
 * @returns {Promise<void>} A promise that resolves once all questions are added to the quiz document.
 * @throws {Error} If there is an error adding questions to the quiz document. 
 */
async function addQuizQuestions() {
    var courseID = localStorage.getItem("ID");
    var quizName = document.getElementById("quiz-name").value;
    const quizDocRef = doc(db, courseID, 'Quizzes', 'all-quizzes', quizName);


    var questionInputs = $('.question-inputs');

    questionInputs.each(async function () {
        var inputQuestion = $(this).find('.question').val();
        var correctAns = $(this).find('.correct-answer').val();
        var incorrectAnswer1 = $(this).find('.incorrect-answer1').val();
        var incorrectAnswer2 = $(this).find('.incorrect-answer2').val();
        var incorrectAnswer3 = $(this).find('.incorrect-answer3').val();

        var newQuestion = {
            correctAnswer: correctAns,
            incorrectAnswers: [incorrectAnswer1, incorrectAnswer2, incorrectAnswer3],
            question: inputQuestion
        };

        await updateDoc(quizDocRef, {
            questions: arrayUnion(newQuestion),
        });

        // Clear question inputs
        $(this).find('input').val('');
    });

    var confirmationMessageElement = document.getElementById("confirmation-message");
    confirmationMessageElement.textContent = "Question submitted successfully!";
    confirmationMessageElement.style.display = "block";


    setTimeout(function () {
        confirmationMessageElement.style.display = "none";
    }, 2800); // timeout 2800 milliseconds


    console.log("Document updated")
}


/**
 * Fetches all quizzes associated with the current course ID from the database and displays them as buttons on the page.
 * 
 * @function getAllQuizzes
 * @returns {void} This function doesn't return a value directly, but fetches and displays quiz buttons asynchronously.
 * @throws {Error} If there is an error fetching or processing quiz data from the database.
 */
function getAllQuizzes() {
    var courseID = localStorage.getItem("ID");
    console.log(courseID);

    // Reference to the Quizzes collection in the database
    const docRef = doc(db, courseID, "Quizzes");
    const docSnap = collection(docRef, "all-quizzes");

    const row = document.getElementById('quizzes');

    // Fetch all documents in the "all-quizzes" subcollection
    getDocs(docSnap).then((querySnapshot) => {
        querySnapshot.forEach((doc) => {
            const quizID = doc.id;
            const button = document.createElement("button");
            button.innerHTML = quizID;
            button.classList.add("btn", "quizOption-button");
            button.style.display = "inline-block";

            //Add click event listener to each quiz button
            button.addEventListener("click", function () {

                localStorage.setItem("selectedQuizID", quizID);

                window.location.href = "quiz.html";
            });

            // Creat card container
            const card_container = document.createElement("div");
            card_container.classList.add("card", "container", "mt-5", "border", "border-dark");
            card_container.style.width = "25rem";

            //creat card body
            const card_body = document.createElement("div");
            card_body.classList.add("card-body");


            //create heart
            const heart = document.createElement("p");
            heart.classList.add("btn", "shadow-none");
            getLikeFromQuiz(quizID).then(likeCount => {
                heart.textContent = "🖤" + likeCount;
            });
            heart.style.fontSize = "2rem";
            heart.style.display = "inline-block";

            heart.addEventListener('click', async () => {
                try {
                    const count = await getLikeFromQuiz(quizID);
                    console.log(count);
                    if (heart.textContent == "🖤" + count) {

                        await uppdateLikes(count + 1, quizID);
                        heart.textContent = "❤" + await getLikeFromQuiz(quizID);

                    }
                    else {
                        await uppdateLikes(count - 1, quizID);
                        heart.textContent = "🖤" + await getLikeFromQuiz(quizID);
                    }
                } catch (error) {
                    console.error("error", error);
                }
            });

            //append all
            card_body.appendChild(button);
            card_body.appendChild(heart);
            card_container.appendChild(card_body);
            row.appendChild(card_container);

        });
    }).catch((error) => {
        console.log("Error getting documents: ", error);
    });
}

/**
 * Uppdates the number of likes a quiz has
 * 
 * @function shuffleArray
 * @param {number} count - The number to change the likes to
 * @param {string} quizID - The name of the quiz
 * @returns {void} This function do not return anything 
 */
async function uppdateLikes(count, quizID) {
    //Retrieve the course id from localStorage
    var courseID = localStorage.getItem("ID");
    console.log(courseID);

    //Reference to the quizzes collection in the database
    const docRef = doc(db, courseID, "Quizzes");

    //Reference to the specific quiz document
    const docSnap = collection(docRef, "all-quizzes");
    console.log(docSnap);

    //Get the quiz document snapshot
    const quizDocRef = doc(docSnap, quizID);
    const quizDocSnap = await getDoc(quizDocRef);

    if (quizDocSnap.exists()) { // Check if the document snapshot exists
        // Update the likes count in the document
        await updateDoc(quizDocRef, {
            likes: count
        });
    } else {
        console.log("No such document!");
    }
}

/**
 * Gets the number of likes a quiz has
 * 
 * @function getLikeFromQuiz
 * @param {string} quizID - The name of the quiz
 * @returns {number} - returns the number of likes
 */
async function getLikeFromQuiz(quizID) {

    //Retrieve the course id from localStorage
    var courseID = localStorage.getItem("ID");
    console.log(courseID);

    //Reference to the quizzes collection in the database
    const docRef = doc(db, courseID, "Quizzes");

    //Reference to the specific quiz document
    const docSnap = collection(docRef, "all-quizzes");
    console.log(docSnap);

    //Get the quiz document snapshot
    const quizDocRef = doc(docSnap, quizID);
    const quizDocSnap = await getDoc(quizDocRef);

    if (docSnap) {
        return quizDocSnap.data().likes;
    } else {
        console.log("No such document!");
    }

}


/**
 * Shuffles the elements of an array in place using the Fisher-Yates shuffle algorithm.
 * 
 * @function shuffleArray
 * @param {Array} array - The array to be shuffled
 * @returns {void} This function shuffles the array in place and does not return a new array. 
 */
function shuffleArray(array) {
    for (let i = array.length - 1; i > 0; i--) {
        const j = Math.floor(Math.random() * (i + 1));
        [array[i], array[j]] = [array[j], array[i]];
    }
}


/**
 * Asynchronously fetches a quiz from the database based on the provided quiz ID.
 * 
 * @async
 * @function fetchQuiz
 * @param {string} quizID - The ID of the quiz to fetch
 * @returns {Promise<Array<Object>>} A promise that resolves with an array of objects representing quiz questions.
 * @throws {Error} If there is an error in fetching the quiz or processing the data.
 */
async function fetchQuiz(quizID) {
    //Retrieve the course id from localStorage
    var courseID = localStorage.getItem("ID");
    console.log(courseID);

    //Reference to the quizzes collection in the database
    const docRef = doc(db, courseID, "Quizzes");

    //Reference to the specific quiz document
    const docSnap = collection(docRef, "all-quizzes");
    console.log(docSnap);

    //Get the quiz document snapshot
    const quizDocRef = doc(docSnap, quizID);
    const quizDocSnap = await getDoc(quizDocRef);

    //Extract questions data from the quiz document
    const questionsData = quizDocSnap.data().questions;
    let docQuestions = [];

    //Process each question in the quiz       
    if (questionsData) {
        questionsData.forEach((questionItem) => {
            const correctAnswer = questionItem.correctAnswer;
            const wrongAnswersMap = questionItem.incorrectAnswers;
            const questionText = questionItem.question;

            const wrongAnswersArray = Object.values(wrongAnswersMap);

            //Combine correct and incorrect answers, shuffle them
            var allAnswers = [correctAnswer, ...wrongAnswersArray];
            shuffleArray(allAnswers);

            //Add question data to the array
            docQuestions.push({
                question: questionText,
                answers: allAnswers,
                correctAnswer: correctAnswer
            });

        });
        console.log(docQuestions);
    }
    //Return the array of processed quiz questions  
    return docQuestions;
}


let currentQuestionIndex = 0;   //Position of the current question
let questionNumber = 1;   //Enumerates questions in order of quiz
let score = 0;   //Final score for current quiz
let allQuestionlength = 0;   //length of all questions
var ID;   //ID for current quiz
let questions = [];   //Array with all questions

/**
* Starts a quiz by fetching quiz questions and displaying them one by one. 
* 
* @function startQuiz
* @param {string} quizID - The ID of the quiz to start. 
* @returns {void} This function displays quiz questions and manages the quiz flow asynchronously.
* @throws {Error} If there is an error fetching or processing quiz questions.
*/
async function startQuiz(quizID) {

    let quizQuestion = await fetchQuiz(quizID);
    questionNumber = 1;
    currentQuestionIndex = 0;
    score = 0;
    allQuestionlength = quizQuestion.length;
    ID = quizID;
    nextButton.innerHTML = "Next";

    for (let i = 0; i < quizQuestion.length; i++) {

        questions = quizQuestion[i];

        showQuestion(quizQuestion[i]);

        await new Promise(resolve => {
            nextButton.onclick = resolve;

        });
    }
    showScore();
}


/**
* Displays the current quiz question with answer option on the page.
* 
* @function showQuestion
* @param {Object} arr - An object containing the current question and it's answer options.
* @returns {void} This function updates the DOM to show the current questions and answer options.
*/
function showQuestion(arr) {
    resetState();
    let currentQuestions = arr;
    let questionNo = questionNumber++;
    questionElem.innerHTML = questionNo + ". " + currentQuestions.question;

    currentQuestions.answers.forEach(answer => {
        const button = document.createElement("button");
        button.innerHTML = answer;
        button.classList.add("btn");
        answerButtons.appendChild(button);
        if (answer === currentQuestions.correctAnswer) {
            button.dataset.correct = true;
        }
        button.addEventListener("click", selectAnswer)
    });
}


/**
* Resets the state of the quiz by hiding the next button and removing all answer buttons from the DOM.
* 
* @function resetState
* @returns {void} This function updates the DOM to reset the quiz state. 
*/
function resetState() {
    nextButton.style.display = "none";
    while (answerButtons.firstChild) {
        answerButtons.removeChild(answerButtons.firstChild);
    }
}


/**
* Handles the user's selection of an answer for a quiz question.
* 
* @function selectAnswer
* @param {Event} e - The event object representing the click event on an answer button.
* @returns {void} This function updates the DOM to reflect the user's answer selection and enables the next button.  
*/
function selectAnswer(e) {
    const selectedBtn = e.target;

    const isCorrect = selectedBtn.innerHTML === questions.correctAnswer;
    if (isCorrect) {
        selectedBtn.classList.add("correct");
        score++;
    } else {
        selectedBtn.classList.add("incorrect");
    }
    // Disable all buttons after selection
    Array.from(answerButtons.children).forEach(button => {
        if (button.innerHTML === questions.correctAnswer) {
            button.classList.add("correct");
        }
        button.disabled = true;
    });
    nextButton.style.display = "block";
}


/**
* Displays the final score of the quiz and allow the user to quit or return to the quiz menu.
* 
* @function showScore
* @returns {void} This function updates the DOM to show the user's final score and provides options to quit or return to the quiz menu. 
*/
function showScore() {
    resetState();
    questionElem.innerHTML = `You scored ${score} out of ${allQuestionlength}!`;
    nextButton.innerHTML = "Quit";
    nextButton.style.display = "block";

    console.log(ID);
    nextButton.addEventListener("click", function () {
        window.location.href = "quiz_menu.html";
    });
}


/**
* Handles the "Next" button functionality during the quiz.
* 
* @function handleNextButton
* @returns {void} This function updates the current question index and checks if the quiz is complete to show the final score.
*/
function handleNextButton() {
    currentQuestionIndex++
    if (currentQuestionIndex > allQuestionlength) {
        showScore();
    }
}


//Assigning functions to global variables for ease of access

/**
 * Function reference for fetching a quiz based on its ID.
 * 
 * @type {Function}
 */
window.fetchQuiz = fetchQuiz;

/**
 * Function reference for starting a quiz based on its ID.
 * 
 * @type {Function}
 */
window.startQuiz = startQuiz;

/**
 * Function reference for fetching all quizzes associated with the current course.
 * 
 * @type {Function}
 */
window.getAllQuizzes = getAllQuizzes;

/**
 * Function reference for for creating a new quiz.
 * @type {Function}
 */
window.createQuiz = createQuiz;

/**
 * Function reference for adding questions to an existing quiz.
 * 
 * @type {Function}
 */
window.addQuizQuestions = addQuizQuestions;
