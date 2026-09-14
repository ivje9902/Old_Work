// Function to handle form submission
function submitValues_testversion() {
    var actorText = document.getElementById("actorButton").innerText || actorText.textContent;
    var normalizedactorText = actorText.replace(/\s+/g, ' ').trim();
    var houseText = document.getElementById("houseButton").innerText || houseText.textContent;
    var normalizedhouseText = houseText.replace(/\s+/g, ' ').trim();
    var warmText = document.getElementById("varmButton").innerText || warmText.textContent;
    var normalizedwarmText = warmText.replace(/\s+/g, ' ').trim();
    var energyText = document.getElementById("energiButton").innerText || energyText.textContent;
    var normalizedenergyText = energyText.replace(/\s+/g, ' ').trim();
    var ventilationText = document.getElementById("ventButton").innerText || ventilationText.textContent;
    var normalizedventilationText = ventilationText.replace(/\s+/g, ' ').trim();

    var energikonsumption = document.getElementById("energyInput").value;
    var storlek = document.getElementById("sizeInput").value;
    var byggnadsår = document.getElementById("yearInput").value;
    var kommun = document.getElementById("kommunInput").value;
    var cbox1 = document.getElementById("cbox1").checked;
    var cbox2 = document.getElementById("cbox2").checked;
    var övrigt = document.getElementById("extraInput").value;

    if (energikonsumption < 0 || storlek < 0 || byggnadsår < 0) {
        alert("Ogiltigt värde, fyll i positiva heltal");
        return;
    }

    else if (
        normalizedactorText == "Vilken typ av aktör är du?" ||
        normalizedhouseText == "Typ av byggnad" ||
        normalizedwarmText == "Uppvärmningsmetod" ||
        normalizedenergyText == "Energisystem" ||
        normalizedventilationText == "Typ av ventilation" ||
        energikonsumption.length <= 0 ||
        storlek.length <= 0 ||
        byggnadsår.length <= 0 ||
        kommun.length <= 0
    ) {
        alert("Fyll i alla fälten för bästa resultat");
    }

    else if (
        byggnadsår > parseInt(document.getElementById('yearInput').max)
    ) {
        alert("Max värde för byggnadsår har överskridits");

    }

    else {
        setCookie("actor", normalizedactorText, 30);
        setCookie("house", normalizedhouseText, 30);
        setCookie("method", normalizedwarmText, 30);
        setCookie("system", normalizedenergyText, 30);
        setCookie("cbox1", cbox1, 30);
        setCookie("cbox2", cbox2, 30);
        setCookie("vent", normalizedventilationText, 30);
        setCookie("energy", energikonsumption, 30);
        setCookie("size", storlek, 30);
        setCookie("kommun", kommun, 30);
        setCookie("year", byggnadsår, 30);
        setCookie("övrigt", övrigt, 30);

        console.log("Actor: " + normalizedactorText);
        console.log("Byggnadstyp: " + normalizedhouseText);
        console.log("Uppvärmningsmetod: " + normalizedwarmText);
        console.log("Golvvärme: " + cbox1);
        console.log("Radiatorer: " + cbox2);
        console.log("Ventilation: " + normalizedventilationText);
        console.log("Energisystem: " + normalizedenergyText);
        console.log("Energikonsumption: " + energikonsumption);
        console.log("Storlek: " + storlek);
        console.log("Kommun: " + kommun);
        console.log("Byggnadsår: " + byggnadsår);
        console.log("Övrigt: " + övrigt);
        console.log("Cookies set");

        window.location.href = '/comparison';

    }

}

function createTipElements_testversion(tips, containerId = "tips-container") {
    const tipsContainer = document.getElementById(containerId);
    tipsContainer.innerHTML = ""; // Clear previous tips

    let selectedTips = [];
    let selectedIndices = [];

    const tipElements = tips.map((tip, index) => {
        const { title, description } = tip;

        // Create the box wrapper
        const boxWrapper = document.createElement("div");
        boxWrapper.className = "tip-box";
        boxWrapper.style.border = "1px solid #ccc";
        boxWrapper.style.padding = "10px";
        boxWrapper.style.marginBottom = "10px";
        boxWrapper.style.borderRadius = "5px";
        boxWrapper.style.backgroundColor = "LightBlue";
        boxWrapper.style.display = "flex";
        boxWrapper.style.alignItems = "flex-start";
        boxWrapper.style.gap = "10px";
        boxWrapper.style.cursor = "default";
        boxWrapper.style.flexDirection = "row";

        // Checkbox (checked and disabled)
        const checkbox = document.createElement("input");
        checkbox.type = "checkbox";
        checkbox.checked = true;
        checkbox.disabled = true;
        checkbox.style.marginTop = "5px";

        // Add to selected arrays by default
        selectedTips.push(tip);
        selectedIndices.push(index);

        // Content container
        const contentDiv = document.createElement("div");
        contentDiv.style.flex = "1";

        // Title of the tip
        const titleDiv = document.createElement("div");
        titleDiv.className = "tip-title";
        titleDiv.textContent = title;
        titleDiv.style.fontWeight = "bold";
        titleDiv.style.fontSize = "1.1em";

        // Description of the tip
        const descDiv = document.createElement("div");
        descDiv.className = "tip-description";
        descDiv.textContent = description;
        descDiv.style.marginTop = "4px";
        descDiv.style.color = "#333";

        // Place where the expanded answers will be
        const extraDiv = document.createElement("div");
        extraDiv.className = "tip-extra";
        extraDiv.style.maxHeight = "0";
        extraDiv.style.overflow = "hidden";
        extraDiv.style.transition = "max-height 0.3s cubic-bezier(0.4,0,0.2,1)";
        extraDiv.style.color = "#222";
        extraDiv.style.marginTop = "8px";
        extraDiv.textContent = "Här kommer mer information om denna åtgärd.";

        const expandedText = extraDiv;

        // Assemble
        contentDiv.appendChild(titleDiv);
        contentDiv.appendChild(descDiv);
        contentDiv.appendChild(extraDiv);

        boxWrapper.appendChild(checkbox);
        boxWrapper.appendChild(contentDiv);

        // Collapsible logic
        boxWrapper.addEventListener("click", function () {
            const isActive = boxWrapper.classList.toggle("active");
            if (isActive) {
                extraDiv.style.maxHeight = extraDiv.scrollHeight + "px";
            } else {
                extraDiv.style.maxHeight = "0";
            }
        });

        tipsContainer.appendChild(boxWrapper);

        return { boxWrapper, expandedText };
    });


    /*
    offertButton.addEventListener("click", () => {
        document.getElementById("loading-offert-button").style.display = "inline-block";
        offertButton.style.display = "none";
        generateOffert(selectedTips);
    });

    guideButton.addEventListener("click", () => {
        const guideLoader = document.getElementById("loading-guide-button");
        guideLoader.style.display = "inline-block";
        guideButton.style.display = "none";
        generateGuide(selectedTips);
    });

    energySavingButton.addEventListener("click", () => {
        const savingLoader = document.getElementById("loading-saving-button");
        savingLoader.style.display = "inline-block";
        energySavingButton.style.display = "none";
        generateSaving(selectedTips);
    });
    */

    return {
        tipElements,
        selectedTips,
        selectedIndices
    };
}
// Function to get response from GPT API
async function gptResponse_testversion(questionType, inputs) {
    if (!questionType || !inputs) {
        return res.status(400).json({ error: "Error in fields" });
    }

    req_body = JSON.stringify({ inputs: inputs });

    let ans;

    try {
        if (localStorage.getItem(req_body)) {
            console.log("Answer already in local storage");
            ans = JSON.parse(localStorage.getItem(req_body));
            localStorage.setItem("req_body", req_body);
        } else {
            localStorage.setItem("req_body", req_body);

            const response = await fetch(`/response/${questionType}`, {
                method: "POST",
                headers: {
                    "Content-Type": "application/json",
                },
                body: req_body
            });


            ans = await response.json();
        }

        let ansResult, ragResult;

        if (questionType == "question") {
            localStorage.setItem("req_body", req_body);
            localStorage.setItem(req_body, JSON.stringify(ans));
            console.log(req_body, "Answer saved in local storage.")

            console.log(ans, "Answer from API");

            ansResult = createTipElements_testversion(ans.tips, "tips-container-ans");
            ragResult = createTipElements_testversion(ans.tipsRAG, "tips-container-rag");

            document.getElementById("loader").style.display = "none";

            const ansTipsWithSource = ansResult.selectedTips.map((tip, i) => ({
                ...tip,
                source: "ans",
                localIndex: i
            }));
            const ragTipsWithSource = ragResult.selectedTips.map((tip, i) => ({
                ...tip,
                source: "rag_ans",
                localIndex: i
            }));


            const developButton = document.getElementById("develop-button");
            const loadingButton = document.getElementById("loading-button");


            developButton.onclick = async () => {
                const allSelectedTips = ansTipsWithSource.concat(ragTipsWithSource);
                if (allSelectedTips.length > 0) {
                    developButton.style.display = "none";
                    loadingButton.style.display = "inline-block";
                    await expandText_testversion(
                        allSelectedTips,
                        ansResult,
                        ragResult
                    );
                    loadingButton.style.display = "none";
                    developButton.style.display = "inline-block";
                }
            };
        }
    } catch (error) {
        console.error(error);
    }
}

function load_page_testversion() {
    console.log("Page swap successful");

    var byggnadstyp = getCookie("house");
    var metod = getCookie("method");
    var energisystem = getCookie("system");
    var energikonsumption = getCookie("energy");
    var storlek = getCookie("size");
    var byggnadsår = getCookie("year");
    var ventilation = getCookie("vent");
    var kommun = getCookie("kommun");
    var extra = getCookie("övrigt");
    var actor = getCookie("actor");
    var cbox1 = getCookie("cbox1");
    var cbox2 = getCookie("cbox2");

    // Both rag and no-RAG will be used
    const mode = "both"

    const inputs = {
        byggnadstyp: byggnadstyp,
        Uppvärmningsmetod: metod,
        energisystem: energisystem,
        energikonsumption: energikonsumption,
        storlek: storlek,
        byggnadsår: byggnadsår,
        golvvärme: cbox1,
        radiatorer: cbox2,
        ventilation: ventilation,
        kommun: kommun,
        övrigt: extra,
        aktör: actor,
        version: window.version,
        mode: mode
    };

    gptResponse_testversion("question", inputs)

}


async function expandText_testversion(allSelectedTips, ansResult, ragResult) {
    console.log(allSelectedTips, "allSelectedTips");

    const ans = JSON.parse(localStorage.getItem(req_body));
    const context = ans.tips;

    // Separate inputs for NO RAG and RAG
    const ansSelectedTips = allSelectedTips.filter(tip => tip.source === "ans");
    const ragSelectedTips = allSelectedTips.filter(tip => tip.source === "rag_ans");

    const inputs = {
        context: context,
        ansInput: ansSelectedTips,
        ragInput: ragSelectedTips,
        mode: "both",
    };
    console.log(inputs, "Inputs for expansion");

    // Call GPT backend for expansion
    const response = await fetch(`/response/expand`, {
        method: "POST",
        headers: { "Content-Type": "application/json" },
        body: JSON.stringify({ inputs: inputs })
    });
    const data = await response.json();

    // Map source to expanded answers and tipElements
    const MAP = {
        ans: {
            expanded: data.expanded,
            tipElements: ansResult.tipElements,
        },
        rag_ans: {
            expanded: data.expandedRAG,
            tipElements: ragResult.tipElements,
        }
    };

    console.log(data, "Expanded data");

    // Map expanded answers to correct tips using source and localIndex
    allSelectedTips.forEach((tip) => {
        const src = tip.source;
        const map = MAP[src];
        if (map && map.expanded && map.tipElements) {
            const tidIndex = tip.localIndex;
            const tipElement = map.tipElements[tidIndex];
            // Use localIndex to get the correct expanded answer
            if (map.expanded[tidIndex]) {
                tipElement.expandedText.textContent = map.expanded[tidIndex].description;
                tipElement.boxWrapper.classList.add("active");
                tipElement.expandedText.style.maxHeight = tipElement.expandedText.scrollHeight + "px";
            }
        }
    });
}

window.submitValues_testversion = submitValues_testversion;

