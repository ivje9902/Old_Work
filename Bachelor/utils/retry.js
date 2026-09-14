/**
 * Retries an asynchronous API call with exponential backoff in case of failure.
 * 
 * @param {Function} fn - The asynchronous function to execute.
 * @param {string} action - A description of the action being performed (used for logging).
 * @param {number} [retries=10] - The maximum number of retry attempts.
 * @param {number} [delay=2000] - The initial delay (in milliseconds) between retry attempts.
 * @returns {Promise<*>} A promise that resolves to the result of the function if successful.
 * @throws {Error} If all retry attempts fail or the error is unrecoverable.
 */
async function retryApiCall(fn, action, retries = 10, delay = 2000) {
    console.log("Trying API call...");
    for (let attempt = 1; attempt <= retries; attempt++) {
        console.log(`Attempt ${attempt} for ${action}...`);
        try {
            return await fn();
        } catch (error) {
            if (attempt === retries) {
                console.error("Max retries reached. Throwing error.");
                if (typeof alert === "function") {
                    alert("Just nu är modellen överbelastad, försök igen om en stund.");
                }
                throw error; 
            }

            if(error.code === 503) {
                console.error("Model is overloaded");
            }
            
            console.error(`Attempt ${attempt} failed for promptRAG:`, error.message);
            console.log(`Retrying in ${delay}ms...`);
            await new Promise(resolve => setTimeout(resolve, delay)); 
            delay *= 2; 
        }
    }
}

module.exports = {
    retryApiCall
};