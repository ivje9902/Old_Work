// Initialize the dropdowns

/**
 * Function to toggle the dropdown buttons and handling input fields within dropdowns.
 * The function adds event listeners to dropdowns to show the dropdown button that was clicked and hide others.
 * @param {NodeList} dropdownButtons - The list of dropdown buttons
 * @param {NodeList} dropdownContents - The list of dropdown contents
 */
function toggleDropdown(dropdownButtons, dropdownContents) {
    dropdownButtons.forEach((button) => {
        button.addEventListener('click', (event) => {
            // Close all other dropdowns
            dropdownContents.forEach((content) => {
                if (content !== button.nextElementSibling) {
                    content.style.display = 'none';
                }
            });

            // Toggle the current dropdown
            const dropdownContent = button.nextElementSibling;
            dropdownContent.style.display =
                dropdownContent.style.display === 'block' ? 'none' : 'block';

            
            event.stopPropagation();
            });
    });
}

/**
 * This function allows users to fill the "other" text prompt in the dropdown.
 * When the user fills the "other" text input and presses the Enter key,
   the input value is used to update the corresponding dropdown button's name.
 * @param {NodeList} otherInputs - The list of other input fields 
 */
function setupDropdowns(otherInputs) {
    otherInputs.forEach((input) => {
        //Call changeName function when pressing enter
        input.addEventListener('keydown', (event) => {
            if (event.key === 'Enter') {
                const dropdownContent = input.parentElement;
                const buttonId = input.id.replace('other', '').toLowerCase() + 'Button';
                changeName(buttonId, input.value);
                input.value = '';
                dropdownContent.style.display = "none"; // Hide the input field
            }
        });

        input.addEventListener('click', (event) => {
            event.stopPropagation(); // Prevent the click event from propagating
        });
    });
}

/**
 * Function to set up checkboxes in dropdowns.
 * Function adds event listener to checkboxes to to prevent the dropdown to close when clicking on them.
 * @param {NodeList} cboxInputs - The list of checkbox inputs
 */
function setupCboxes(cboxInputs) {
    cboxInputs.forEach((cbox) => {
        cbox.addEventListener('click', (event) => {
            event.stopPropagation(); // Prevent the click event from propagating
        });
    });
}

/**
 * Function to close all dropdowns.
 * @param {NodeList} dropdownContents - The list of dropdown contents 
 */
function closeDropdowns(dropdownContents) {
    dropdownContents.forEach((content) => {
        content.style.display = 'none';
    });
}


/**
 * Function to initialize the dropdowns.
 * Function sets up the dropdown buttons, other inputs, and checkboxes.
 */
function initDropdowns() {
    const dropdownButtons = document.querySelectorAll('.dropbtn');
    const dropdownContents = document.querySelectorAll('.dropdown-content');
    const otherInputs = document.querySelectorAll('.other_input');
    const cboxInputs = document.querySelectorAll('.cbox_input');

    toggleDropdown(dropdownButtons, dropdownContents);
    setupDropdowns(otherInputs);
    setupCboxes(cboxInputs);
    
    // Close dropdowns when clicking outside
    document.addEventListener('click', () => {
        closeDropdowns(dropdownContents, null);
    });
}

/**
 * Function that changes the name of the dropdown button.
 * The name is the text that is displayed on the button.
 * @param {String} id - The id of the button that is to be changed
 * @param {String} string - The text that is displayed on button
 */
function changeName(id, string) {
    const text = document.getElementById(id)

    text.innerHTML = string + '&nbsp' + '<i class="fa-solid fa-caret-down"></i>';
}

// Initialize the dropdowns when page is loaded
document.addEventListener('DOMContentLoaded', initDropdowns);


