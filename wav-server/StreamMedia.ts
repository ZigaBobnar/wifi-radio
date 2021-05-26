import * as fs from "fs";
import { WaveFile } from "wavefile";

/**
 * Container for media that is processed into such format that it can be
 * streamed (8-bit WAV with correct samplerate).
 */
class StreamMedia {
  isLoaded: boolean = false;
  samples: Uint8Array = new Uint8Array();
  samplesCount: number = 0;
  chunkCount: number = 0;
  totalLengthMs: number = 0;

  constructor(
    public path: string,
    public sampleRate: number,
    public chunkSize: number,
    deferredLoad: boolean = false
  ) {
    if (!deferredLoad) {
      this.loadMedia(true);
    }
  }

  getChunk(chunkIndex: number): Uint8Array {
    return this.samples.slice(
      chunkIndex * this.chunkSize,
      (chunkIndex + 1) * this.chunkSize
    );
  }

  loadMedia(forceReload: boolean = false) {
    if (this.isLoaded && !forceReload) return;

    const wavFile = new WaveFile(fs.readFileSync(this.path));
    wavFile.toBitDepth("8", true);
    wavFile.toSampleRate(this.sampleRate);

    this.samples = new Uint8Array(wavFile.getSamples(false, Int8Array));
    this.samplesCount = this.samples.length;
    this.chunkCount = Math.ceil(this.samplesCount / this.chunkSize);
    this.totalLengthMs = (1000 * this.samples.length) / this.sampleRate;
    this.isLoaded = true;
  }
}

export { StreamMedia };
