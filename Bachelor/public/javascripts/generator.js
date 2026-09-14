const { jsPDF } = window.jspdf;
const { marked } = window.marked;
window.html2canvas = html2canvas;


/**
 * Makes a call to the API 
 * 
 * @param {Array} selectedText - The selected tips that will be sent to the API.
 * @param {string} type - What type of backend function is used, depending on what generator function called it
 * 
 * @returns {string} - Returns the response from the API call, in markdown format. Depending on type the response can also include latex.
 */
async function ai_response(selectedText, type) {
    const context = JSON.parse(req_body).inputs;
    const mode = JSON.parse(req_body).inputs.mode;
    const inputarr = [];

    selectedText.forEach(element => {
        inputarr.push(element.title);
    });

    const inputs = {
        context: context,
        input: inputarr,
        mode: mode,
    };

    const response = await fetch(`/response/` + type, {
        method: "POST",
        headers: {
            "Content-Type": "application/json",
        },
        body: JSON.stringify({ inputs: inputs })
    });

    const offer = await response.json();
    console.log("offer:", offer);
    const txt = offer.answer;
    return txt;
}

/**
 * Changes the text inside of the visual textbox to display whatever response was received from the API, and displays it.
 * 
 * @param {string} markdownTxt - The response from the API that is to be displayed.
 */
function display_response(markdownTxt) {
    const textBox = document.getElementById("evolve_text");

    textBox.innerHTML = marked(markdownTxt);


    MathJax.typesetPromise([textBox]).then(() => {
    }
    ).catch((error) => {
        console.error("MathJax typeset error:", error);
    });

    document.getElementById("evolve_parent").style.display = "block";
    textBox.classList.remove("invisible");

    const collection = document.getElementsByTagName("button");
    Array.from(collection).forEach(function (element) {
        element.disabled = false
    });

    return;
}

/**
 * Creates the download button, and adds the content of the visual textbox into a downloadable PDF file.
 * Depending on what type of content is in the visual textbox he function will create the document in different ways. 
 * This is because some types contains latex that have to be handled differently.
 * If a download button already exists, it first removes the old one before creating the new one.
 * 
 * @param {string} type - Specifies what type of pdf is created, depending on what generate function called it.
 */
function create_download(type) {

    if (document.getElementById("download-button")) {
        document.getElementById("download-button").remove();
    }

    const downloadButton = document.createElement("button");

    downloadButton.id = "download-button";
    downloadButton.className = "btn";
    downloadButton.classList.add("mt-2", "px-10", "py-5", "w-1/5", "text-lg", "font-bold", "text-gray-900", "bg-white", "border", "border-gray-200", "rounded-s-lg", "rounded-e-lg", "hover:bg-gray-100", "hover:text-blue-700", "focus:z-10", "focus:ring-2", "focus:ring-blue-700", "focus:text-blue-700");
    downloadButton.innerHTML = '<i class="fa fa-download"></i> Ladda ner';
    downloadButton.style.display = "inline-block";
    document.body.appendChild(downloadButton);

    if (type == "Saving" && getBrowser() != "Firefox") {
        downloadButton.addEventListener("click", () => {

            const doc = new jsPDF();
            const element = document.getElementById("evolve_text");



            html2canvas(element).then(canvas => {
                const imgData = canvas.toDataURL('image/png');
                const imgWidth = doc.internal.pageSize.getWidth() - 20;
                const imgHeight = canvas.height * imgWidth / canvas.width;

                doc.addImage(imgData, 'PNG', 20, 20, imgWidth, imgHeight);
                doc.save(type + ".pdf");
            });
        });
    }

    else if (type == "Saving" && getBrowser() == "Firefox") {
        downloadButton.addEventListener("click", () => {

            const doc = new jsPDF();
            const element = document.getElementById("evolve_text");
            let textcontent = element.textContent;
            textcontent = textcontent.replace(/ä([A-Z])/g, '$1');

            console.log("InnerText:", element.innerText);
            console.log("Textcontent:", element.textContent);

            doc.setFontSize(10);
            const pagewidth = doc.internal.pageSize.getWidth() - 40;

            const text = doc.splitTextToSize(textcontent, pagewidth);

            doc.text(text, 20, 20);

            doc.save(type + ".pdf");

        });
    }

    else if (type == "Offer-template") {
        downloadButton.addEventListener("click", () => {
            const doc = new jsPDF();
            const txt = document.getElementById("evolve_text").innerText;

            doc.setFontSize(10);
            const pagewidth = doc.internal.pageSize.getWidth() - 40;

            const text = doc.splitTextToSize(txt, pagewidth);

            doc.text(text, 20, 20);

            doc.save(type + ".pdf");

        });
    }

    else if (type == "Guide") {
        downloadButton.addEventListener("click", () => {
            let x = 20;
            let y = 20;

            const doc = new jsPDF();
            const txt = document.getElementById("evolve_text").textContent;

            doc.setFontSize(10);
            const pagewidth = doc.internal.pageSize.getWidth() - 40;
            const pageheight = doc.internal.pageSize.getHeight() - 20;

            const text = doc.splitTextToSize(txt, pagewidth);
            let textHeight = doc.getTextDimensions(text).h;
            console.log("Page height:", pageheight);
            console.log("Text Height:", doc.getTextDimensions(text).h);

            if (textHeight > pageheight) {
                text_array = txt.split("\n");
                text_array.forEach((line) => {
                    textHeight = doc.getTextDimensions(line).h;
                    let text_line = doc.splitTextToSize(line, pagewidth);
                    if (y + textHeight > pageheight) {
                        doc.addPage();
                        x = 20;
                        y = 20;
                    }
                    doc.text(text_line, x, y);
                    y += doc.getTextDimensions(text_line).h + 5;

                });

            }
            else {
                doc.text(text, x, y);

            }

            doc.save(type + ".pdf");

        });
    }

    else {

        downloadButton.addEventListener("click", () => {

            const doc = new jsPDF();
            const txt = document.getElementById("evolve_text").innerHTML;

            doc.setFontSize(10);
            const pagewidth = doc.internal.pageSize.getWidth() - 20;
            const pageheight = doc.internal.pageSize.getHeight() - 20;

            const words = txt.split("<p>");
            words.shift();

            let bold = false;
            let x = 20;
            let y = 20;

            words.forEach(word => {

                word = word.replace("</p>", "");

                if (word.includes("<strong>")) {
                    bold = true;

                    word = word.replace("</strong>", "");
                    word = word.replace("<strong>", "");
                }

                // Calculate how many lines tall the paragraph will be
                const lines = Math.ceil(doc.getTextDimensions(word).w / pagewidth);

                // Calculate how high a single line is
                const wordHeight = Math.ceil(doc.getTextDimensions(word).h);

                // Take those two values to calcuate the height of the entire paragraph
                const paragraphHeight = lines * wordHeight;


                if (bold) {
                    doc.setFont(undefined, "bold");
                }


                if (y + paragraphHeight > pageheight) {
                    doc.addPage();
                    x = 20;
                    y = 20;
                }

                if (y + (2 * paragraphHeight) > pageheight && bold) {
                    doc.addPage();
                    x = 20;
                    y = 20;
                }

                const text = doc.splitTextToSize(word, pagewidth - 20);
                doc.text(text, x, y);
                y += (5 * lines) + 5;


                doc.setFont(undefined, "normal");
                bold = false;

            });

            doc.save(type + ".pdf");
        });
    }

}

/**
 * Retrieves an array of all elements with the tag "button". 
 * Loops through each button and disables them.
 * 
 * @returns {void}
 */
function disable_buttons() {
    const collection = document.getElementsByTagName("button");
    Array.from(collection).forEach(function (element) {
        element.disabled = true;
    });
    return;
}

/**
 * Makes a call to the API to expand upon the @param selectedTips and displays it, also creates a download button. 
 * 
 * @param {Array} selectedTips - The selected tips that will be expanded.
 */
async function generateExpand(selectedTips) {

    disable_buttons();

    const req_body = JSON.parse(localStorage.getItem("req_body"));
    const ans = JSON.parse(localStorage.getItem(req_body));
    const inputs = {
        context: req_body.inputs,
        input: selectedTips
    };

    // Call GPT backend for expansion
    const response = await fetch(`/response/expand`, {
        method: "POST",
        headers: { "Content-Type": "application/json" },
        body: JSON.stringify({ inputs: inputs })
    });
    const data = await response.json();
    const expandedAnswers = data.expanded;

    const title = document.getElementById("evolve_title");
    title.innerHTML = 'Här har du mer information om de valda tipsen, du kan skriva fritt i textrutan, och även ladda ner den som PDF';
    title.classList.remove("invisible");

    display_response(expandedAnswers);

    document.getElementById("expandButton").innerHTML = 'Utveckla svaren'

    create_download("Expand");

    title.scrollIntoView({ behavior: "smooth" });
}

/**
 * Generates offer templates for the selected tips, using the API.
 * 
 * @param {Array} selectedText - The selected tips that will be sent to the API.
 */
async function generateOffert(selectedText) {

    disable_buttons();

    const markdownTxt = await ai_response(selectedText, "offert");

    const title = document.getElementById("evolve_title");
    title.innerHTML = 'Här har du din offert-mall, du kan fylla i fälten, och även ladda ner den som PDF';
    title.classList.remove("invisible");

    display_response(markdownTxt);

    document.getElementById("offerButton").innerHTML = 'Offertförfrågan'

    create_download("Offer-template");

    title.scrollIntoView({ behavior: "smooth" });
}

/**
 * Generates step-by-step guides for the selected tips, using the API.
 * 
 * @param {Array} selectedText - The selected tips that will be sent to the API.
 */
async function generateGuide(selectedText) {

    disable_buttons();

    const markdownTxt = await ai_response(selectedText, "guide");

    const title = document.getElementById("evolve_title");
    title.innerHTML = 'Här har du din steg-för-steg guide, du kan skriva fritt i textfältet, och även ladda ner den som PDF';
    title.classList.remove("invisible");

    display_response(markdownTxt);

    document.getElementById("guideButton").innerHTML = 'Steg för steg guide';

    create_download("Guide");

    title.scrollIntoView({ behavior: "smooth" });
}

/**
 * Generates energy saving calculations for the selected tips, using the API.
 * 
 * @param {Array} selectedText - The selected tips that will be sent to the API.
 */
async function generateSaving(selectedText) {

    disable_buttons();

    const markdownTxt = await ai_response(selectedText, "saving");

    const title = document.getElementById("evolve_title");
    title.innerText = "Här är din energibesparing, du kan skriva fritt i textfältet, och även ladda ner den som PDF";
    title.classList.remove("invisible");

    display_response(markdownTxt);

    document.getElementById("calculationButton").innerHTML = 'Energibesparing';

    create_download("Saving");

    title.scrollIntoView({ behavior: "smooth" });
}



