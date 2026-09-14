import { initializeApp } from 'firebase/app';
import { getFirestore, collection, addDoc, getDocs, query, where, doc, updateDoc, deleteDoc } from 'firebase/firestore';
jest.mock('firebase/app');
jest.mock('firebase/firestore');
  
initializeApp.mockReturnValue({});

const db = getFirestore();

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

async function updateDocument(collectionName, docId, data) {
  try {
    const washingtonRef = doc(db, collectionName, docId);
    await updateDoc(washingtonRef, data);
    console.log("Update completed");
  } catch (error) {
    console.error("Error updating: ", error);
  }
}

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

async function deleteDocument(collectionName, docId) {
  try {
    await deleteDoc(doc(db, collectionName, docId));
    console.log("Deletion completed");
  } catch (error) {
    console.error("Error deleting: ", error);
  }
}

const testCollectionName = "testCollection";
const testDocumentId = "testDocumentId";
const testDocumentData = { name: "Test Document" };
const testUpdateData = { name: "Updated Test Document" };
const testCollectionName2 = "testCollection2";
const testDocumentId2 = "testDocumentId2";
const testDocumentData2 = { name: "Test Document2" };
const testUpdateData2 = { name: "Updated Test Document2" };

describe("Database Tests", () => {
  test("Create Document", async () => {
    addDoc.mockImplementation(() => Promise.resolve({ id: testDocumentId }));

    const docId = await createDocument(testCollectionName, testDocumentData);

    expect(docId).toEqual(testDocumentId);
    expect(addDoc).toHaveBeenCalledWith(collection(db, testCollectionName), testDocumentData);

    addDoc.mockImplementation(() => Promise.resolve({ id: testDocumentId2 }));

    const docId2 = await createDocument(testCollectionName2, testDocumentData2);

    expect(docId2).toEqual(testDocumentId2);
    expect(addDoc).toHaveBeenCalledWith(collection(db, testCollectionName2), testDocumentData2);
  });

  test("Update Document", async () => {
    updateDoc.mockImplementation(() => Promise.resolve());

    await updateDocument(testCollectionName, testDocumentId, testUpdateData);

    expect(updateDoc).toHaveBeenCalledWith(doc(db, testCollectionName, testDocumentId), testUpdateData);

    updateDoc.mockImplementation(() => Promise.resolve());

    await updateDocument(testCollectionName2, testDocumentId2, testUpdateData2);

    expect(updateDoc).toHaveBeenCalledWith(doc(db, testCollectionName2, testDocumentId2), testUpdateData2);
  });

  test("Read Documents", async () => {
    getDocs.mockImplementation(() => Promise.resolve({
      forEach: (callback) => callback({ id: testDocumentId, data: () => testDocumentData }),
    }));

    await readDocuments(testCollectionName, "field", "value");

    expect(getDocs).toHaveBeenCalledWith(query(collection(db, testCollectionName), where("field", "==", "value")));
    
    getDocs.mockImplementation(() => Promise.resolve({
      forEach: (callback) => callback({ id: testDocumentId2, data: () => testDocumentData2 }),
    }));

    await readDocuments(testCollectionName2, "field", "value");

    expect(getDocs).toHaveBeenCalledWith(query(collection(db, testCollectionName2), where("field", "==", "value")));
  });

  test("Delete Document", async () => {
    deleteDoc.mockImplementation(() => Promise.resolve());

    await deleteDocument(testCollectionName, testDocumentId);

    expect(deleteDoc).toHaveBeenCalledWith(doc(db, testCollectionName, testDocumentId));


    deleteDoc.mockImplementation(() => Promise.resolve());

    await deleteDocument(testCollectionName2, testDocumentId2);

    expect(deleteDoc).toHaveBeenCalledWith(doc(db, testCollectionName2, testDocumentId2));
  });
});
