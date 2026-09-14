/**
 * Function to that activates the upload button when a file is selected.
 * Function checks if the file is a valid filetype, pdf or excel.
 */
function handleFileChange() {
    const fileInput = document.getElementById('submittedFile');
    const uploadButton = document.getElementById('upload-button');
    const fileTypes = ['application/pdf', 'application/vnd.openxmlformats-officedocument.spreadsheetml.sheet']; // PDF and Excel MIME types
    const isValidFile = fileInput.files.length > 0 && fileTypes.includes(fileInput.files[0].type);

    uploadButton.disabled = !isValidFile; 
}

/**
 * Function that displays the name of the selected file in input field for uploading
 */
function showFileName() {
    const fileInput = document.getElementById('submittedFile');
    const fileNameDisplay = document.getElementById('file-name');

    if (fileInput.files.length > 0) {
        fileNameDisplay.textContent = fileInput.files[0].name; 
    } else {
        fileNameDisplay.textContent = 'Click to upload'; 
    }
}


/**
 * Function that handles the file upload.
 * Function sends the file to the server using fetch API.
 * @param {File} file - The file to be uploaded
 * @returns {Promise} a promise that resolves the response from server
 */
async function uploadFile(file) {
    const formData = new FormData();
    formData.append('file', file);

    const response = await fetch('upload/upload-file', {
        method: 'POST',
        body: formData,
    });

    if (!response.ok) {
        const errorData = await response.json(); 
        throw new Error(errorData.error || 'Ett okänt fel inträffade.'); 
    }

    return response.json();
}

/**
 * Function to handle uploading files.
 * Function is called when the upload button is clicked.
 * The function fetches the response from server and displays the results.
 * @param {Event} event 
 * @returns {void}
 */
async function handleFileUpload(event) {
    event.preventDefault();

    const fileInput = document.getElementById('submittedFile');
    const uploadButton = document.getElementById('upload-button');
    const file = fileInput.files[0];

    if (!file) {
        alert('Vänligen välj en fil att ladda upp.');
        return;
    }

    uploadButton.disabled = true;
    uploadButton.textContent = 'Genererar...';

    try {
        const result = await uploadFile(file);

        if (result.apiResponse) {
            displayResults(result.apiResponse);
        } else {
            alert('Ingen data mottogs från servern.');
        }

        console.log("File upload successful");
    } catch (error) {
        alert(error.message);
        console.error('Error during file upload:', error);
    } finally {
        uploadButton.disabled = false;
        uploadButton.textContent = 'Generera Analys';
    }
}
