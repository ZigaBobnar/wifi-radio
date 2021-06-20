import { Request } from "express";

async function sleep(time_ms: number) {
  return new Promise((resolve) => setTimeout(resolve, time_ms));
}

// Taken from https://werxltd.com/wp/2010/05/13/javascript-implementation-of-javas-string-hashcode-method/
function simpleHash(value: string): string {
  let hash = 0;

  if (value.length == 0) {
    return `${hash}`;
  }

  for (let i = 0; i < value.length; i++) {
    let char = value.charCodeAt(i);
    hash = (hash << 5) - hash + char;
    hash = hash & hash; // Convert to 32bit integer
  }

  return `${hash}`;
}

function getClientId(req: Request): string {
  return req.query.clientId
    ? `${req.query.clientId}`
    : simpleHash(req.socket.remoteAddress ?? "");
}

export { sleep, simpleHash, getClientId };
