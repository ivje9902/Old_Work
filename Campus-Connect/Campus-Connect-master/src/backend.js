/**
* Initializes the Firebase app with the provided configuration.
* @function
* @param {Object} firebaseConfig - The Firebase configuration object.
* @returns {Object} The initialized Firebase app.
*/
import { initializeApp } from 'firebase/app';
import { createUserWithEmailAndPassword, getAuth, onAuthStateChanged } from 'firebase/auth';

/**
 * Gets the Firestore database instance.
 * @function
 * @returns {Object} The Firestore database instance.
 */
import { getFirestore, collection, addDoc, getDocs, query, where, doc, setDoc, updateDoc, deleteDoc } from 'firebase/firestore';


/**
 * Gets the Firestore storage instance.
 * @function
 * @returns {Object} The Firestore storage instance.
 */
import { getStorage, ref, exists, getDownloadURL, uploadString, uploadBytes } from 'firebase/storage'

// Firebase configuration object
const firebaseConfig = {
  apiKey: "AIzaSyAjWnGy2HpTFM-07fRp3VIokULmU_dyMg4",
  authDomain: "campusconnect-30c4a.firebaseapp.com",
  databaseURL: "https://campusconnect-30c4a-default-rtdb.europe-west1.firebasedatabase.app",
  projectId: "campusconnect-30c4a",
  storageBucket: "campusconnect-30c4a.appspot.com",
  messagingSenderId: "728123265116",
  appId: "1:728123265116:web:2d24f83a222e156fe4d699",
  measurementId: "G-YRMP4KDMNX"
};

// Initialize Firebase app
const firebaseApp = initializeApp(firebaseConfig);

// Get Firestore database instance
const db = getFirestore();

const auth = getAuth(firebaseApp);

// Get a reference to the storage service
const storage = getStorage();

export { storage, db };

let currentUser = null;

onAuthStateChanged(auth, async (user) => {
  if (user) {
    currentUser = { uid: user.uid, umail: user.email };
    const userRef = doc(db, "users", user.uid);

    const updateData = {
      email: user.email,
    };

    await setDoc(userRef, updateData, { merge: true });

    console.log("User data stored/updated in Firestore.");

    var username = document.getElementById("displayName");
    username.textContent = user.displayName;

  } else {
    currentUser = null;
    console.log("No user is signed in.");
  }
});

export async function getCurrentUser() {
  return currentUser;
}

/**
 * Asynchronously retrieves the download URL for a file stored in Firebase Storage based on a given path.
 * This function creates a reference to a storage location using the provided path, attempts to get the download URL,
 * and logs the URL or an error if the operation fails.
 *
 * @async
 * @function getFileDownloadURL
 * @param {string} path - The path in Firebase Storage from which to retrieve the file's download URL.
 * @returns {Promise<string|null>} The download URL of the file if successful, otherwise null.
 */
export async function getFileDownloadURL(path) {
  try {
    const fileRef = ref(storage, path);
    const downloadURL = await getDownloadURL(fileRef);
    console.log("Download URL:", downloadURL);
    return downloadURL;
  } catch (error) {
    console.error("Error getting download URL:", error);
    return null;
  }
}

/**
 * Asynchronously retrieves all courses from a Firestore collection named "kurser".
 * The function queries the Firestore collection, extracts data from each document,
 * and returns it as an array of objects, each representing a course.
 * 
 * @async
 * @function getCourses
 * @returns {Promise<Array<Object>>} An array of objects, each representing a course, extracted from Firestore documents.
 */
export async function getCourses() {
  const snapshot = await getDocs(collection(db, "kurser"));
  return snapshot.docs.map(doc => doc.data());
}

/**
 * Creates a new reference to the specified tenta.
 * @async
 * @function
 * @param {string} courseID - The ID of given course.
 * @param {string} category - The category/subfolder of the file.
 * @param {string} fileName - The name of the file.
 * @returns {Object|null} - Returns a reference to the file in Firebase Storage if it exists,
 * or null if the file does not exist or an error occurs.
 */
export async function getFileRef(courseID, category, fileName) {
  try {

    const tentaRef = ref(storage, `${courseID}/${category}/${fileName}`);

    console.log("Reference created");
    return tentaRef;

  } catch (error) {
    console.error("Error creating reference:", error);
    return null;
  }
}

/**
 * Uploads a file and creates a firestore reference.
 * @async
 * @function
 * @param {string} collectionID - The collection ID.
 * @param {string} category - The type of file.
 * @param {string} fileName - File name, included ."type" (Example: .PDF).
 * @param {string} desc - The file description.
 * @param {File} file - The file object selected by the user.
 */
async function uploadFile(collectionID, category, fileName, desc, file) {
  try {
    const storageRef = ref(storage, `${collectionID}/${category}/${fileName}`);
    addArrayFieldToDocument(collectionID, category, fileName, `${collectionID}/${category}/${fileName}`, desc);
    await uploadBytes(storageRef, file).then((snapshot) => {
      console.log("Uploaded file succesfully");
    });
  } catch (error) {
    console.error("Error uploading file:", error);
  }
}

/**
 * Adds an array field with information inside a given document in a given collection.
 * @async
 * @function
 * @param {string} collectionID - The collection ID.
 * @param {string} documentName - The document name.
 * @param {string} fieldValue1 - Array index 0 value.
 * @param {string} fieldValue2 - Array index 1 value.
 * @param {string} fieldValue3 - Array index 2 value.
 */
async function addArrayFieldToDocument(collectionID, documentName, fieldValue1, fieldValue2, fieldValue3) {
  try {
    const docRef = doc(db, collectionID, documentName);
    await setDoc(docRef, {
      [fieldValue1]: [fieldValue1, fieldValue2, fieldValue3]
    }, { merge: true });
    console.log("Array field added/updated successfully");
  } catch (error) {
    console.error("Error adding array field:", error);
  }
}

/**
 * Creates a new folder and subfolders for a course.
 * @async
 * @function
 * @param {string} collectionID - The ID of the collection.
 */
async function storageCreateCourse(collectionID) {
  try {
    const storage = getStorage();
    const bucket = ref(storage);

    // Create the root folder
    const rootFolderPath = `${collectionID}/`;

    // List of subfolders to be created in the new course folder
    const subfolderNames = ['Kursmaterial', 'Quiz', 'Tentor', 'Videor'];

    // Iterate through the list of subfolder names and create them within the root folder
    for (const subfolderName of subfolderNames) {
      const subfolderPath = `${rootFolderPath}${subfolderName}/`;
      await uploadString(ref(bucket, `${subfolderPath}dummyFile`), '');
      console.log("Subfolder created (or already exists):", subfolderPath);
    }
  } catch (error) {
    console.error("Error creating folders:", error);
  }
}



/**
 * Adds a new document to the specified collection.
 * @async
 * @function
 * @param {string} collectionName - The name of the collection.
 * @param {Object} data - The data to be added to the document.
 * @returns {string|null} The ID of the newly created document, or null if an error occurred.
 */
async function createDocument(collectionName, data) {
  try {
    const docRef = await addDoc(collection(db, collectionName), data);
    console.log("Document created", docRef.id);
    return docRef.id;
  } catch (error) {
    console.error("Error creating ", error);
    return null;
  }
}

/**
 * Updates an existing document in the specified collection.
 * @async
 * @function
 * @param {string} collectionName - The name of the collection.
 * @param {string} docId - The ID of the document to be updated.
 * @param {Object} data - The updated data for the document.
 */
async function updateDocument(collectionName, docId, data) {
  try {
    const washingtonRef = doc(db, collectionName, docId);
    await updateDoc(washingtonRef, data);
    console.log("Update completed");
  } catch (error) {
    console.error("Error updating: ", error);
  }
}

/**
 * Reads documents from the specified collection based on a specific field and value.
 * @async
 * @function
 * @param {string} collectionName - The name of the collection.
 * @param {string} field - The field to filter documents by.
 * @param {any} value - The value to filter documents by.
 */
async function readDocuments(collectionName, field, value) {
  try {
    const q = query(collection(db, collectionName), where(field, "==", value));
    const querySnapshot = await getDocs(q);
    querySnapshot.forEach((doc) => {
      console.log(doc.id);
    });
  } catch (error) {
    console.error("Error reading: ", error);
  }
}

/**
 * Deletes a document from the specified collection.
 * @async
 * @function
 * @param {string} collectionName - The name of the collection.
 * @param {string} docId - The ID of the document to be deleted.
 */
async function deleteDocument(collectionName, docId) {
  try {
    await deleteDoc(doc(db, collectionName, docId));
    console.log("Deletion completed");
  } catch (error) {
    console.error("Error deleting: ", error);
  }
}

// Expose createDocument function globally for usage
window.createDocument = createDocument;

// Expose getFileDownloadURL function globally for usage
window.getFileDownloadURL = getFileDownloadURL;

// Expose storageCreateCourse function globally for usage
window.storageCreateCourse = storageCreateCourse;

// Expose uploadFile function globally for usage
window.uploadFile = uploadFile;

window.getCurrentUser = getCurrentUser;