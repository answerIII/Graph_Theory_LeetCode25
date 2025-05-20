import React from "react";
import { Container, Typography } from "@mui/material";
import GraphUploader from "../components/GraphUploader";

const Home: React.FC = () => {
    return (
        <Container>
            <Typography variant="h3" gutterBottom align="center">
                Graph Theory project
            </Typography>
            <GraphUploader />
        </Container>
    );
};

export default Home;