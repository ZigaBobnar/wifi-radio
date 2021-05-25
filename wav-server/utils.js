async function sleep(time_ms) {
    return new Promise(resolve => setTimeout(resolve, time_ms));
}

module.exports = {
    sleep
}
