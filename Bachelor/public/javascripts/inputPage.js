/**
 * Creates a cookie for a given name, value, and expiration days.
 * Saves the cookie in the browser.
 * 
 * @param {String} cname - Name of the cookie
 * @param {any} cvalue - Value of the cookie
 * @param {Int} exdays - Number of days until the cookie expires
 */
function setCookie(cname, cvalue, exdays) {
    const d = new Date();
    d.setTime(d.getTime() + (exdays * 24 * 60 * 60 * 1000));
    let expires = "expires=" + d.toUTCString();
    document.cookie = cname + "=" + cvalue + ";" + expires + ";path=/";
}

// Function to handle form submission
/**
 * Handles the button press "Generate".
 * Gets the values from the input fields and sets them as cookies.
 * Checks if the values are valid, and if not, alerts the user.
 * Changes the page to the style page.
 * 
 */
function submitValues() {
    var houseText = document.getElementById("houseButton").innerText || houseText.textContent;
    var normalizedhouseText = houseText.replace(/\s+/g, ' ').trim();
    var warmText = document.getElementById("varmButton").innerText || warmText.textContent;
    var normalizedwarmText = warmText.replace(/\s+/g, ' ').trim();
    var ventilationText = document.getElementById("ventButton").innerText || ventilationText.textContent;
    var normalizedventilationText = ventilationText.replace(/\s+/g, ' ').trim();

    var energikonsumption = document.getElementById("energyInput").value || "Vet ej";
    var energienhet = document.getElementById("energyButton").innerText || "";
    var normalzedEnergyText = energienhet.replace(/\s+/g, ' ').trim();
    var energi = energikonsumption + " " + normalzedEnergyText;
    var storlek = document.getElementById("sizeInput").value || "Vet ej";
    var byggnadsår = document.getElementById("yearInput").value || "Vet ej";
    var kommun = document.getElementById("kommunInput").value || "Vet ej";
    var cbox1 = document.getElementById("cbox1").checked;
    var cbox2 = document.getElementById("cbox2").checked;
    var cbox3 = document.getElementById("cbox3").checked;
    var cbox4 = document.getElementById("cbox4").checked;
    var cbox5 = document.getElementById("cbox5").checked;
    var cbox6 = document.getElementById("cbox6").checked;
    var cbox7 = document.getElementById("cbox7").checked;
    var cbox8 = document.getElementById("cbox8").checked;
    var cbox9 = document.getElementById("cbox9").checked;
    var övrigt = document.getElementById("extraInput").value || "Ingen Text";


    if (energikonsumption < 0 || storlek < 0 || byggnadsår < 0) {
        alert("Ogiltigt värde, fyll i positiva heltal");
        return;
    }

    else if (
        byggnadsår > parseInt(document.getElementById('yearInput').max)
    ) {
        alert("Max värde för byggnadsår har överskridits");

    }

    else {

        if (normalizedhouseText !== "Typ av byggnad") { setCookie("house", normalizedhouseText, 30); }

        if (normalizedwarmText !== "Uppvärmningssystem") { setCookie("method", normalizedwarmText, 30); }

        if (normalizedventilationText !== "Typ av ventilation") { setCookie("vent", normalizedventilationText, 30); }

        if (energikonsumption !== "Vet ej") { setCookie("energy", energi, 30); 
                                              setCookie("enhet", energienhet, 30); }

        if (storlek !== "Vet ej") { setCookie("size", storlek, 30); }

        if (kommun !== "Vet ej") { setCookie("kommun", kommun, 30); }

        if (byggnadsår !== "Vet ej") { setCookie("year", byggnadsår, 30); }

        for (let i = 1; i <= 9; i++) {
            setCookie("cbox" + i, document.getElementById("cbox" + i).checked, 30);
        }

        if (övrigt !== "Ingen Text") { setCookie("övrigt", övrigt, 30); }

        window.location.href = '/style';
    }

}

/**
 * Update the text content of the elements
 * 
 * @param {String} type 
 * @param {String} method 
 * @param {String} system 
 * @param {Int} energy 
 * @param {Int} size 
 * @param {Int} year 
 */
function updateText(type, method, system, energy, size, year) {

    document.getElementById("type").textContent = type;
    document.getElementById("method").textContent = method;
    document.getElementById("system").textContent = system;
    document.getElementById("size").textContent = size;
    document.getElementById("year").textContent = year;

    // Remove the 'hidden' class to display the paragraphs
    document.getElementById("type").classList.remove("hidden");
    document.getElementById("method").classList.remove("hidden");
    document.getElementById("system").classList.remove("hidden");
    document.getElementById("size").classList.remove("hidden");
    document.getElementById("year").classList.remove("hidden");

}

/**
 * Shows the cookie consent message after the page loads.
 */
document.addEventListener('DOMContentLoaded', () => {
    const cookieConsent = document.getElementById('cookieConsent');
    const acceptCookies = document.getElementById('acceptCookies');

    // Check if the user has already accepted cookies
    if (!localStorage.getItem('cookieConsent')) {
        cookieConsent.style.display = 'block';
    }

    // Handle the "Accept" button click
    acceptCookies.addEventListener('click', () => {
        const cookies = {
            "cookieConsent": true,
        }
        localStorage.setItem("cookieConsent", cookies);
        cookieConsent.style.display = 'none'; // Hide the consent message
    });
});



