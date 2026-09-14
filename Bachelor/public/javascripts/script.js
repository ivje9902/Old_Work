window.version = "1.0.7";

// type "no-RAG" for call without RAG
// type "with-RAG" for call with RAG
window.currentMode = "no-RAG";

console.log("Version: " + version);

/**
 * Function to delete all cookies.
 */
function deleteCookies() {
    let allCookies = document.cookie.split(';');

    // Delete cookies by modifying the expiry to a past date
    for (let i = 0; i < allCookies.length; i++)
        document.cookie = allCookies[i] + "=;expires="
            + new Date(0).toUTCString();

    console.log("All cookies deleted");
}

/**
 * Function to reset all input fields and dropdowns to their default values.
 * It also deletes all cookies and resets checkboxes.
 */
function resetValues() {
    // Reset all text inputs
    const textInputs = document.querySelectorAll("input[type='text'], input[type='number']");
    textInputs.forEach((input) => {
        input.value = "";
    });

    deleteCookies();

    // Reset dropdowns to their default text
    const dropdownButtons = document.querySelectorAll(".dropbtn");
    dropdownButtons.forEach((button) => {

        if (button.id === "houseButton") {
            button.innerHTML = 'Typ av byggnad' + '&nbsp;' + '<i class="fa-solid fa-caret-down"></i>';
        } else if (button.id === "varmButton") {
            button.innerHTML = 'Uppvärmningssystem' + '&nbsp;' + '<i class="fa-solid fa-caret-down"></i>';
        } else if (button.id === "fastighetVärme") {
            button.innerHTML = 'Installationer i anslutning till byggnaden &nbsp;<i class="fa-solid fa-caret-down"></i>';
        } else if (button.id === "ventButton") {
            button.innerHTML = 'Typ av ventilation' + '&nbsp;' + '<i class="fa-solid fa-caret-down"></i>';
        } else if (button.id === "energyButton") {
            button.innerHTML =  'kW/h' + '&nbsp' + '<i class="fa-solid fa-caret-down"></i>';
        }
    });

    // Reset checkboxes
    const checkboxes = document.querySelectorAll("input[type='checkbox']");
    checkboxes.forEach((checkbox) => {
        checkbox.checked = false;
    });
}

/**
 * Tries to checks what version of the website the user is on. 
 * If the version is not the same as the one in localStorage, it removes all keys from localStorage.
 * If it fails it will not remove any keys, but instead display an error message in the console.
 * 
 * @returns {void}
 */
function checkVersion() {
    let allCookies = document.cookie.split(';');
    Object.keys(localStorage).forEach(function (key) {
        try {
            if (key === "cookieConsent" || key === "req_body") {
                return;
            }

            const parsedKey = JSON.parse(key);

            const versionInKey = parsedKey.inputs.version || undefined;
            const modeInKey = parsedKey.inputs.mode || undefined;

            if (!versionInKey || versionInKey !== version || !modeInKey || modeInKey !== currentMode) {
                console.log("Removing key: " + key);
                localStorage.removeItem(key);

                if (allCookies.length !== 0) {
                    deleteCookies();
                    resetValues();
                }
            }
        } catch (error) {
            console.error(`Error parsing localStorage key "${key}":`, error);
        }
    });
}

/**
 * An eventlistener that calls checkVersion as soon as the DOM is loaded.
 * This is to ensure that the function is called as soon as possible.
 */
document.addEventListener("DOMContentLoaded", () => { checkVersion(); });

/**
 * Changes the name of the input fields in the index.html file once an option is selected from the dropdown menu.
 * 
 * @param {Number} id - The id of the element to change.
 * @param {String} string - What value to change the element to.
 * 
 * @returns {void}
 */
function changeName(id, string) {
    const text = document.getElementById(id)

    text.innerHTML = string + '&nbsp;' + '<i class="fa-solid fa-caret-down"></i>';

}


/**
 * Retrieves a value from a cookie by its name.
 * 
 * @param {String} cname 
 * 
 * @returns {any} - The value of the cookie, or an empty string if not found.
 */
function getCookie(cname) {

    let name = cname + "=";
    let decodedCookie = decodeURIComponent(document.cookie);
    let ca = decodedCookie.split(';');
    for (let i = 0; i < ca.length; i++) {
        let c = ca[i];
        while (c.charAt(0) == ' ') {
            c = c.substring(1);
        }
        if (c.indexOf(name) == 0) {
            return c.substring(name.length, c.length);
        }
    }
    return "";
}

/**
 * Loads the index page, sets the starting strings of all the input fields. 
 * As well as checking for any previous values in cookies, and if found sets the input fields to those values.
 * 
 * @returns {void}
 */
function load_index() {
    const startValues = [
        "Typ av byggnad",
        "Uppvärmningssystem",
        "Typ av ventilation",
        "Vet ej",
        "Ingen Text",
    ];

    const cookieMappings = [
        { cookieName: "house", elementId: "houseButton", defaultValue: "Typ av byggnad" },
        { cookieName: "method", elementId: "varmButton", defaultValue: "Uppvärmningssystem" },
        { cookieName: "vent", elementId: "ventButton", defaultValue: "Typ av ventilation" },
        { cookieName: "kommun", elementId: "kommunInput", isInput: true },
        { cookieName: "year", elementId: "yearInput", isInput: true },
        { cookieName: "energy", elementId: "energyInput", isInput: true },
        { cookieName: "size", elementId: "sizeInput", isInput: true },
        { cookieName: "extra", elementId: "extraInput", isInput: true },
    ];

    cookieMappings.forEach(({ cookieName, elementId, defaultValue, isInput }) => {
        let cookieValue = getCookie(cookieName);
        const element = document.getElementById(elementId);

        // Extract numeric value for energy
        if (cookieName === "energy" && cookieValue) {
            const matches = cookieValue.match(/(\d+)/);
            cookieValue = matches ? matches[0] : "";
        }
        
        if (element) {
            if (isInput) {
                element.value = cookieValue || "";
            } else {
                if (!startValues.includes(cookieValue) && cookieValue !== "") {
                    element.innerHTML = cookieValue.trim() + '&nbsp;' + '<i class="fa-solid fa-caret-down"></i>';
                } else {
                    element.innerHTML = defaultValue + '&nbsp;' + '<i class="fa-solid fa-caret-down"></i>';
                }
            }
        }
    });

    // Set energyButton text only once, after the loop
    var enhet = getCookie("enhet");
    document.getElementById("energyButton").innerHTML = enhet ?
        enhet.trim() + '&nbsp;' + '<i class="fa-solid fa-caret-down"></i>' :
        'kW/h' + '&nbsp;' + '<i class="fa-solid fa-caret-down"></i>';

    if (getCookie("cbox1") === "true") {
        document.getElementById("cbox1").checked = true;
    }
    if (getCookie("cbox2") === "true") {
        document.getElementById("cbox2").checked = true;
    }
    if (getCookie("cbox3") === "true") {
        document.getElementById("cbox3").checked = true;
    }
    if (getCookie("cbox4") === "true") {
        document.getElementById("cbox4").checked = true;
    }
    if (getCookie("cbox5") === "true") {
        document.getElementById("cbox5").checked = true;
    }
    if (getCookie("cbox6") === "true") {
        document.getElementById("cbox6").checked = true;
    }
    if (getCookie("cbox7") === "true") {
        document.getElementById("cbox7").checked = true;
    }
    if (getCookie("cbox8") === "true") {
        document.getElementById("cbox8").checked = true;
    }
    if (getCookie("cbox9") === "true") {
        document.getElementById("cbox9").checked = true;
    }

    toggleInfoBox();
}

function toggleSubmenu(event) {
    event.preventDefault(); // Prevent link navigation

    const submenu = event.target.nextElementSibling;
    submenu.classList.toggle('show');
}

function closeSubmenu() {
    document.querySelectorAll('.submenu').forEach(menu => {
        menu.classList.remove('show');
    });
}



