const {generateEmbeddings, vectorSearch} = require('./embeddingHandle.js');

async function searchDatabase(queryPrompt, topK = 10) {
    const queryEmbedding = await generateEmbeddings(queryPrompt);
    const results = await vectorSearch(queryEmbedding, topK);
    return results;
}

function displayResults(results, textPrintLimit = 2 ) {
    let resToDisplay = `\n🎯 Top ${results.length} relevant results:\n`;
    for (let i = 0; i < results.length; i++) {
        const res = results[i];
        resToDisplay += `(${i + 1})\nID: ${res.id}\n`;
        resToDisplay += `Similarity: ${res.similarity?.toFixed(4)}\n`;
        resToDisplay += `Source: ${res.metadata?.source}\n`;

        if (i < textPrintLimit) {
            resToDisplay += `Text: ${res.text.substring(0, 150).trim()}...\n`;
        }
        resToDisplay += `\n`;
    }
    console.log(resToDisplay);
    return resToDisplay;
}

function formatResults(results) {
    return results.map((res, i) => {
        return {
            id: res.id,
            similarity: res.similarity?.toFixed(4),
            text: res.text.trim(),
            source: res.metadata
        };
    });
}

module.exports = {
    searchDatabase,
    displayResults,
    formatResults
}

// async function main() {
//     const queryPrompt = `"Vilka är de mest relevanta tipsen för att förbättra energieffektiviteten i en byggnad med följande egenskaper: {
//             byggnadstyp: "Enfamiljshus",
//             Uppvärmningsmetod: "Luft/vattenvärmepump",
//             energisystem: "El",
//             energikonsumption: 200,
//             storlek: 150,
//             byggnadsår: 1999
//         }"`
//     const topK = 5;
//     const results = await searchDatabase(queryPrompt, topK);
//     console.log("🔄 Formatting results...");
//     const resultsFormatted = formatResults(results);
//     console.log("✅ Results formatted.");
//     displayResults(results);
// }

// main()
//     .then(() => console.log('Search completed successfully.'))
//     .catch(err => console.error(err));