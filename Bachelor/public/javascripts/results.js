/**
 * Function to display the results of the uploaded and analyzed file.
 * Function takes the analyzed response from Gemini API and displays it on the page.
 * @param {String} apiResponse - The analyzed response from the API
 * @returns {void}
 */
function displayResults(apiResponse) {
    const resultsContainer = document.getElementById('results-container');
    resultsContainer.innerHTML = ''; // Clear previous results

    if (!apiResponse) {
        resultsContainer.innerHTML = '<p class="text-black-400 italic">No analysis results to display.</p>';
        return;
    }
    
    const cleanedResponse = apiResponse
    .replace(/\*\*/g, '')
    .replace(/^#+\s*/gm, '')
    .replace(/^(\d\.)/gm, '\n$1')
    .replace(/^\*\s*/gm, '• ')
    .replace(/-{3,}/g, '')
    .trim();
    
    const paragraphs = cleanedResponse.split('\n\n'); 

    paragraphs.forEach(paragraph => {
        const paragraphElement = document.createElement('p');
        paragraphElement.textContent = paragraph.trim(); 
        paragraphElement.className = 'text-black-400 italic font-bold'; 
        resultsContainer.appendChild(paragraphElement);
    });
}
