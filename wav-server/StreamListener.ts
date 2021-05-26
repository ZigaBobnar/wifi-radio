/**
 * Stream listener definition. Stream is broadcasted to each listener via its
 * write function.
 */
class StreamListener {
  constructor(
    public name: string,
    public writeFn: (data: Uint8Array) => void
  ) {}
}

export { StreamListener };
