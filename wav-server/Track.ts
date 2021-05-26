import * as fs from "fs";
import { WaveFile } from "wavefile";

/**
 * Container for media that is processed into such format that it can be
 * streamed (8-bit WAV with correct samplerate).
 */
class Track {
  isLoaded: boolean = false;
  samples: Uint8Array = new Uint8Array();
  samplesCount: number = 0;
  chunkCount: number = 0;
  totalLengthMs: number = 0;

  constructor(
    public path: string,
    public sampleRate: number,
    public chunkSize: number,
    deferLoading: boolean = true
  ) {
    if (!deferLoading) {
      this.loadTrack(true);
    }
  }

  getChunk(chunkIndex: number): Uint8Array {
    if (!this.isLoaded) this.loadTrack();

    return this.samples.slice(
      chunkIndex * this.chunkSize,
      (chunkIndex + 1) * this.chunkSize
    );
  }

  loadTrack(forceReload: boolean = false) {
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

export { Track };
