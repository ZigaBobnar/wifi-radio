import { port } from "./config";
import net from "net";

const testEndpoint = "/track/0";
const testMethod = "GET";

const testUrl = `http://localhost:${port}${testEndpoint}`;

console.log(`Test client connecting to ${testUrl}`);

const client = new net.Socket();
client.connect(port, "localhost", () => {
  console.log("Connected");
  client.write(`${testMethod} ${testEndpoint} HTTP/1.0\n\n`);
});

client.on("data", (data) => {
  console.log(
    `Received ${data.length} bytes\n\t> ${data
      .toString()
      .replace("\n", "\n\t> ")}`
  );
});

client.on("close", () => {
  console.log("Connection closed");
});
