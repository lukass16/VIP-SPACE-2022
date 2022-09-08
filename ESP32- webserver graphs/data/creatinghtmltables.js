
window.onload = function()
{
    createTable1();

    // createTable2();

    // createTable3();
}


function createTable1()
{
    const elements = Elements.GetElements();
    const headings = Elements.GetHeadings();
    const properties = Elements.GetProperties();

    // start table and add caption
    let tablehtml = "<table><caption>GPS</caption>";

    // insert row of headings
    tablehtml  += "<tr>";
    for(let heading of headings)
    {
        tablehtml  += `<th>${heading}</th>`;
    }
    tablehtml += "</tr>";

    // iterate data and add row of cells for each
    for(let element of elements)
    {
        tablehtml  += "<tr>";

        for(let property of properties)
        {
            tablehtml  += `<td>${element[property]}</td>`;
        }

        tablehtml  += "</tr>";
    }

    // end of table
    tablehtml += "</table>";

    // add table to the empty div
    document.getElementById("tablediv").innerHTML = tablehtml;
}

function createTable2()
{
    const elements = Elements.GetElements();
    const headings = Elements.GetHeadings();
    const properties = Elements.GetProperties();

    // create table
    const table = document.createElement("table");

    // create caption and add to table
    const caption = document.createElement("caption");
    const captiontext = document.createTextNode("Elements");
    caption.appendChild(captiontext);
    table.appendChild(caption);

    // create row for headings...
    const hrow = document.createElement("tr");
    table.appendChild(hrow);

    // ...and add cells to it
    for(let heading of headings)
    {
        const th = document.createElement("th");
        const thtext = document.createTextNode(heading);
        th.appendChild(thtext);
        hrow.appendChild(th);
    }

    // iterate data, adding rows and cells for each item
    for(let element of elements)
    {
        const drow = document.createElement("tr");
        table.appendChild(drow);

        for(let property of properties)
        {
            const td = document.createElement("td");
            const tdtext = document.createTextNode(element[property]);
            td.appendChild(tdtext);
            drow.appendChild(td);
        }
    }

    // add table to div
    document.getElementById("tablediv").appendChild(table);
}

function createTable3()
{
    const elements = Elements.GetElements();
    const headings = Elements.GetHeadings();
    const properties = Elements.GetProperties();

    // create a table
    const table = document.createElement("table");

    // add a caption
    table.createCaption().textContent = "GPS";

    // insert a row and add headings to it
    const hrow = table.insertRow();
    for(let heading of headings)
    {
        hrow.insertCell(-1).outerHTML = `<th>${heading}</th>`;
    }

    // iterate data, adding rows and cells for each element
    for(let element of elements)
    {
        const drow = table.insertRow(-1);

        for(let property of properties)
        {
            drow.insertCell(-1).innerHTML = element[property];
            // OR
            // drow.insertCell(-1).appendChild(document.createTextNode(element[property]));
        }
    }

    // add table to div
    document.getElementById("tablediv").appendChild(table);
}
